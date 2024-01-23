// Copyright (c) embedded ocean GmbH
#include "TemplateInput.hpp"

#include "Attributes.hpp"
#include "TemplateIoTransaction.hpp"

#include <xentara/config/Context.hpp>
#include <xentara/config/Errors.hpp>
#include <xentara/data/DataType.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/model/ForEachAttributeFunction.hpp>
#include <xentara/model/ForEachEventFunction.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>

namespace xentara::plugins::templateDriver
{
	
using namespace std::literals;

const model::Attribute TemplateInput::kValueAttribute { model::Attribute::kValue, model::Attribute::Access::ReadOnly, data::DataType::kFloatingPoint };

auto TemplateInput::load(utils::json::decoder::Object &jsonObject, config::Context &context) -> void
{
	// Go through all the members of the JSON object that represents this object
	bool ioTransactionLoaded = false;
	for (auto && [name, value] : jsonObject)
    {
		/// @todo use a more descriptive keyword, e.g. "poll"
		if (name == "ioTransaction"sv)
		{
			context.resolve<TemplateIoTransaction>(value, [this](std::reference_wrapper<TemplateIoTransaction> ioTransaction)
				{ 
					_ioTransaction = &ioTransaction.get();
					ioTransaction.get().addInput(*this);
				});
			ioTransactionLoaded = true;
		}
		/// @todo load custom configuration parameters
		else if (name == "TODO"sv)
		{
			/// @todo parse the value correctly
			auto todo = value.asNumber<std::uint64_t>();

			/// @todo check that the value is valid
			if (!"TODO")
			{
				/// @todo use an error message that tells the user exactly what is wrong
				utils::json::decoder::throwWithLocation(value, std::runtime_error("TODO is wrong with TODO parameter of template input"));
			}

			/// @todo set the appropriate member variables
		}
		else
		{
            config::throwUnknownParameterError(name);
		}
    }

	// Make sure that an I/O transaction was specified
	if (!ioTransactionLoaded)
	{
		/// @todo replace "I/O transaction" and "template input" with more descriptive names
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("missing I/O transaction in template input"));
	}
	/// @todo perform consistency and completeness checks
	if (!"TODO")
	{
		/// @todo use an error message that tells the user exactly what is wrong
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("TODO is wrong with template input"));
	}
}

auto TemplateInput::dataType() const -> const data::DataType &
{
	return kValueAttribute.dataType();
}

auto TemplateInput::directions() const -> io::Directions
{
	return io::Direction::Input;
}

auto TemplateInput::forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool
{
	// forEachAttribute() must not be called before references have been resolved, so the I/O transaction should have been
	// set already.
	if (!_ioTransaction) [[unlikely]]
	{
		throw std::logic_error("internal error: xentara::plugins::templateDriver::TemplateInput::forEachAttribute() called before cross references have been resolved");
	}

	return
		// Handle all the attributes we support directly
		function(kValueAttribute) ||

		// Handle the state attributes
		_state.forEachAttribute(function) ||
		// Also handle the common read state attributes from the I/O transaction
		_ioTransaction->forEachReadStateAttribute(function);

	/// @todo handle any additional attributes this class supports, including attributes inherited from the I/O component and the I/O transaction
}

auto TemplateInput::forEachEvent(const model::ForEachEventFunction &function) -> bool
{
	// forEachEvent() must not be called before references have been resolved, so the I/O transaction should have been
	// set already.
	if (!_ioTransaction) [[unlikely]]
	{
		throw std::logic_error("internal error: xentara::plugins::templateDriver::TemplateInput::forEachEvent() called before cross references have been resolved");
	}

	return
		// Handle the state events
		_state.forEachEvent(function, sharedFromThis()) ||
		// Also handle the common read state events from the I/O transaction
		_ioTransaction->forEachReadStateEvent(function);

	/// @todo handle any additional events this class supports, including events inherited from the I/O component and the I/O transaction
}

auto TemplateInput::makeReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
{
	// makeReadHandle() must not be called before references have been resolved, so the I/O transaction should have been
	// set already.
	if (!_ioTransaction) [[unlikely]]
	{
		// Don't throw an exception, because this function is noexcept
		return std::make_error_code(std::errc::invalid_argument);
	}
	// Get the data block
	const auto &dataBlock = _ioTransaction->readDataBlock();
	
	// Handle the value attribute separately
	if (attribute == kValueAttribute)
	{
		return _state.valueReadHandle(dataBlock);
	}
	
	// Handle the state attributes
	if (auto handle = _state.makeReadHandle(dataBlock, attribute))
	{
		return handle;
	}
	// Also handle the common read state attributes from the I/O transaction
	if (auto handle = _ioTransaction->makeReadStateReadHandle(attribute))
	{
		return handle;
	}

	/// @todo handle any additional readable attributes this class supports, including attributes inherited from the I/O component and the I/O transaction

	return std::nullopt;
}

auto TemplateInput::attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	_state.attach(dataArray, eventCount);
}

auto TemplateInput::updateReadState(WriteSentinel &writeSentinel,
	std::chrono::system_clock::time_point timeStamp,
	const utils::eh::expected<std::reference_wrapper<const ReadCommand::Payload>, std::error_code> &payloadOrError,
	const CommonReadState::Changes &commonChanges,
	PendingEventList &eventsToRaise) -> void
{
	// Check if we have a valid payload
	if (payloadOrError)
	{
		/// @todo decode the value from the payload data
		double value = {};

		// Update the state
		_state.update(writeSentinel, timeStamp, value, commonChanges, eventsToRaise);
	}
	// We have an error
	else
	{
		// Update the state with the error
		_state.update(writeSentinel, timeStamp, utils::eh::unexpected(payloadOrError.error()), commonChanges, eventsToRaise);
	}
}

} // namespace xentara::plugins::templateDriver