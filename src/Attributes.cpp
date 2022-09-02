// Copyright (c) embedded ocean GmbH
#include "Attributes.hpp"

#include <xentara/utils/core/Uuid.hpp>
#include <xentara/data/DataType.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver::attributes
{

using namespace std::literals;

const model::Attribute kError { model::Attribute::kError, model::Attribute::Access::ReadOnly, data::DataType::kInteger };

const model::Attribute kWriteError { model::Attribute::kWriteError, model::Attribute::Access::ReadOnly, data::DataType::kInteger };

/// @todo assign a unique UUID
const model::Attribute kConnectionTime { "ffffffff-ffff-ffff-ffff-ffffffffffff"_uuid, u"connectionTime"sv, model::Attribute::Access::ReadOnly, data::DataType::kTimeStamp };

/// @todo assign a unique UUID
const model::Attribute kDeviceError { "abababab-abab-abab-abab-abababababab"_uuid, u"error"sv, model::Attribute::Access::ReadOnly, data::DataType::kInteger };

/// @todo add more offsets for other categories of errors
namespace
{
	/// @brief The size of the slices used to partition the error code range into difrferent error types
	constexpr ErrorCode kErrorCodeSliceSize { 1'000'000'000 };

	/// @brief The offset used for custom error codes
	constexpr ErrorCode kCustomErrorOffset { kErrorCodeSliceSize };

	/// @brief The offset used for custom error codes
	constexpr ErrorCode kUnknownErrorCode { ErrorCode(CustomError::UnknownError) + kCustomErrorOffset };
}
	
auto errorCode(CustomError error) noexcept -> ErrorCode
{
	// Return 0 for OK
	if (error == CustomError::NoError)
	{
		return 0;
	}

	// Just add the offset to the numeric value
	const auto rawErrorCode = std::underlying_type_t<CustomError>(error);

	// Check the range
	if (std::cmp_less(rawErrorCode, 0) || std::cmp_greater_equal(rawErrorCode, kErrorCodeSliceSize))
	{
		return kUnknownErrorCode;
	}

	// Add the offset
	return ErrorCode(rawErrorCode) + kCustomErrorOffset;
}

auto errorCode(std::error_code error) noexcept -> ErrorCode
{
	// If we have no error, use 0
	if (!error)
	{
		return 0;
	}
	
	// Use errors from the system category as-is. These should really never be negative, or too large,
	// but we make sure anyways.
	if (error.category() == std::system_category())
	{
		// Check the range
		if (std::cmp_less(error.value(), 0) || std::cmp_greater_equal(error.value(), kErrorCodeSliceSize))
		{
			return kUnknownErrorCode;
		}

		// USe system errors as-is
		return ErrorCode(error.value());
	}

	// Convert custom errors using the utility function
	if (error.category() == customErrorCategory())
	{
		return errorCode(CustomError(error.value()));
	}

	/// @todo Handle other categories of errors

	// Everything else is an unknown error
	return kUnknownErrorCode;
}

} // namespace xentara::plugins::templateDriver::attributes
