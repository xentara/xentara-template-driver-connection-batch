// Copyright (c) embedded ocean GmbH
#pragma once

#include "CustomError.hpp"

#include <xentara/model/Attribute.hpp>

#include <cstdint>
#include <system_error>

/// @brief Contains the Xentara attributes of the driver
namespace xentara::plugins::templateDriver::attributes
{

/// @brief A Xentara attribute containing a read error code for an I/O point
extern const model::Attribute kError;
/// @brief A Xentara attribute containing a write error code for an I/O point
extern const model::Attribute kWriteError;

/// @brief A Xentara attribute containing the connection time for an I/O component
extern const model::Attribute kConnectionTime;
/// @brief A Xentara attribute containing an error code for an I/O component
extern const model::Attribute kDeviceError;

/// @namespace xentara::plugins::templateDriver::attributes
/// @todo add any additional attributes supplied by the individual elements

/// @brief The type used for error codes in kError and kWriteError
using ErrorCode = std::uint32_t;

/// @brief Generates an error code for a custom error
auto errorCode(CustomError error) noexcept -> ErrorCode;

/// @namespace xentara::plugins::templateDriver::attributes
/// @todo add more functions for other categories of errors, e.g. error codes from a library or protocol

/// @brief Generates an error code for an std::error_code
auto errorCode(std::error_code error) noexcept -> ErrorCode;

} // namespace xentara::plugins::templateDriver::attributes