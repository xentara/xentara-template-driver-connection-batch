// Copyright (c) embedded ocean GmbH
#include "TemplateOutput.hpp"

#include "Attributes.hpp"
#include "TemplateIoBatch.hpp"

#include <xentara/config/Resolver.hpp>
#include <xentara/data/DataType.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/data/WriteHandle.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>

namespace xentara::plugins::templateDriver
{
	
using namespace std::literals;

TemplateOutput::Class TemplateOutput::Class::_instance;

const model::Attribute TemplateOutput::kValueAttribute { model::Attribute::kValue, model::Attribute::Access::ReadWrite, data::DataType::kFloatingPoint };

auto TemplateOutput::loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void
{
	// Get a reference that allows us to modify our own config attributes
    auto &&configAttributes = initializer[Class::instance().configHandle()];

	// Go through all the members of the JSON object that represents this object
	bool ioBatchLoaded = false;
	for (auto && [name, value] : jsonObject)
    {
		/// @todo use a more descriptive keyword, e.g. "poll"
		if (name == u8"ioBatch"sv)
		{
			resolver.submit<TemplateIoBatch>(value, [this](std::reference_wrapper<TemplateIoBatch> ioBatch)
				{ 
					_ioBatch = &ioBatch.get();
					ioBatch.get().addInput(*this);
					ioBatch.get().addOutput(*this);
				});
			ioBatchLoaded = true;
		}
		/// @todo load custom configuration parameters
		else if (name == u8"TODO"sv)
		{
			/// @todo parse the value correctly
			auto todo = value.asNumber<std::uint64_t>();

			/// @todo check that the value is valid
			if (!"TODO")
			{
				/// @todo use an error message that tells the user exactly what is wrong
				utils::json::decoder::throwWithLocation(value, std::runtime_error("TODO is wrong with TODO parameter of template output"));
			}

			/// @todo set the appropriate member variables, and update configAttributes accordingly (if necessary) 
		}
		else
		{
			// Pass any unknown parameters on to the fallback handler, which will load the built-in parameters ("id" and "uuid"),
			// and throw an exception if the key is unknown
            fallbackHandler(name, value);
		}
    }

	// Make sure that an I/O batch was specified
	if (!ioBatchLoaded)
	{
		/// @todo replace "I/O batch" and "template output" with more descriptive names
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("missing I/O batch in template output"));
	}
	/// @todo perform consistency and completeness checks
	if (!"TODO")
	{
		/// @todo use an error message that tells the user exactly what is wrong
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("TODO is wrong with template output"));
	}
}

auto TemplateOutput::dataType() const -> const data::DataType &
{
	return kValueAttribute.dataType();
}

auto TemplateOutput::directions() const -> io::Directions
{
	return io::Direction::Input | io::Direction::Output;
}

auto TemplateOutput::resolveAttribute(std::u16string_view name) -> const model::Attribute *
{
	// resolveAttribute() must not be called before references have been resolved, so the I/O batch should have been
	// set already.
	if (!_ioBatch)
	{
		throw std::logic_error("internal error: xentara::plugins::templateDriver::TemplateInput::resolveAttribute() called before cross references have been resolved");
	}

	// Check all the attributes we support directly
	if (auto attribute = model::Attribute::resolve(name,
		kValueAttribute))
	{
		return attribute;
	}

	// Check the read state attributes
	if (auto attribute = _readState.resolveAttribute(name))
	{
		return attribute;
	}
	// Also check the common read state attributes from the I/O batch
	if (auto attribute = _ioBatch->resolveReadStateAttribute(name))
	{
		return attribute;
	}

	// Check the write state attributes
	if (auto attribute = _writeState.resolveAttribute(name))
	{
		return attribute;
	}

	/// @todo add any additional attributes this class supports, including attributes inherited from the I/O component and the I/O batch

	return nullptr;
}

auto TemplateOutput::resolveEvent(std::u16string_view name) -> std::shared_ptr<process::Event>
{
	// Check the read state events
	if (auto event = _readState.resolveEvent(name, sharedFromThis()))
	{
		return event;
	}
	// Also check the common read state events from the I/O batch
	if (auto event = _ioBatch->resolveReadStateEvent(name))
	{
		return event;
	}

	// Check the write state events
	if (auto event = _writeState.resolveEvent(name, sharedFromThis()))
	{
		return event;
	}

	/// @todo add any additional events this class supports, including events inherited from the I/O component and the I/O batch

	return nullptr;
}

auto TemplateOutput::readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle
{
	// readHandle() must not be called before references have been resolved, so the I/O batch should have been
	// set already.
	if (!_ioBatch)
	{
		// Don't throw an exception, because this function is noexcept
		return std::make_error_code(std::errc::invalid_argument);
	}
	// Get the data blocks
	const auto &readDataBlock = _ioBatch->readDataBlock();
	const auto &writeDataBlock = _ioBatch->writeDataBlock();
	
	// Handle the value attribute separately
	if (attribute == kValueAttribute)
	{
		return _readState.valueReadHandle(readDataBlock);
	}
	
	// Check the read state attributes
	if (auto handle = _readState.readHandle(readDataBlock, attribute))
	{
		return *handle;
	}
	// Also check the common read state attributes from the I/O batch
	if (auto handle = _ioBatch->readStateReadHandle(attribute))
	{
		return *handle;
	}

	// Check the write state attributes
	if (auto handle = _writeState.readHandle(writeDataBlock, attribute))
	{
		return *handle;
	}

	/// @todo add any additional readable attributes this class supports, including attributes inherited from the I/O component and the I/O batch

	return data::ReadHandle::Error::Unknown;
}

auto TemplateOutput::writeHandle(const model::Attribute &attribute) noexcept -> data::WriteHandle
{
	// Handle the value attribute
	if (attribute == kValueAttribute)
	{
		/// @todo use the correct value type
		return { std::in_place_type<double>, &TemplateOutput::scheduleOutputValue, weakFromThis() };
	}

	/// @todo add any additional writable attributes this class supports, including attributes inherited from the I/O component and the I/O batch

	return data::WriteHandle::Error::Unknown;
}

auto TemplateOutput::attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	_readState.attach(dataArray, eventCount);
}

auto TemplateOutput::updateReadState(WriteSentinel &writeSentinel,
	std::chrono::system_clock::time_point timeStamp,
	const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError,
	const CommonReadState::Changes &commonChanges,
	PendingEventList &eventsToFire) -> void
{
	// Check if we have a valid payload
	if (const auto payload = payloadOrError.value())
	{
		/// @todo decode the value from the payload data
		double value = {};

		// Update the read state
		_readState.update(writeSentinel, timeStamp, value, commonChanges, eventsToFire);
	}
	// We have an error
	else
	{
		// Update the read state with the error
		_readState.update(writeSentinel, timeStamp, payloadOrError.error(), commonChanges, eventsToFire);
	}
}

auto TemplateOutput::addToWriteCommand(WriteCommand &command) -> bool
{
	// Get the value
	auto pendingValue = _pendingOutputValue.dequeue();
	// If there was no pending value, do nothing
	if (!pendingValue)
	{
		return false;
	}

	/// @todo add the value to the command

	return true;
}

auto TemplateOutput::attachOutput(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	_writeState.attach(dataArray, eventCount);
}

auto TemplateOutput::updateWriteState(WriteSentinel &writeSentinel,
	std::chrono::system_clock::time_point timeStamp,
	std::error_code error,
	PendingEventList &eventsToFire) -> void
{
	// Update the write state
	_writeState.update(writeSentinel, timeStamp, error, eventsToFire);
}

} // namespace xentara::plugins::templateDriver