// Copyright (c) embedded ocean GmbH
#include "TemplateIoBatch.hpp"

#include "Attributes.hpp"
#include "TemplateInput.hpp"
#include "TemplateOutput.hpp"
#include "WriteCommand.hpp"

#include <xentara/config/Resolver.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/data/WriteHandle.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/process/ExecutionContext.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>
#include <xentara/utils/eh/currentErrorCode.hpp>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

TemplateIoBatch::Class TemplateIoBatch::Class::_instance;

template <typename Buffer>
class TemplateIoBatch::RuntimeBufferSentinel final
{
public:
	/// @brief The constructor clears the buffer of any garbage data that may still be in there
	RuntimeBufferSentinel(Buffer &buffer) : _buffer(buffer)
	{
		_buffer.clear();
	}

	/// @brief The destructor clears the buffer again
	~RuntimeBufferSentinel()
	{
		_buffer.clear();
	}

private:
	/// @brief The buffer to protect
	Buffer &_buffer;
};

auto TemplateIoBatch::loadConfig(const ConfigIntializer &initializer,
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
		/// @todo load configuration parameters
		if (name == u8"TODO"sv)
		{
			/// @todo parse the value correctly
			auto todo = value.asNumber<std::uint64_t>();

			/// @todo check that the value is valid
			if (!"TODO")
			{
				/// @todo use an error message that tells the user exactly what is wrong
				utils::json::decoder::throwWithLocation(value, std::runtime_error("TODO is wrong with TODO parameter of template I/O batch"));
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

	/// @todo perform consistency and completeness checks
	if (!"TODO")
	{
		/// @todo use an error message that tells the user exactly what is wrong
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("TODO is wrong with template I/O batch"));
	}
}

auto TemplateIoBatch::addInput(std::reference_wrapper<AbstractInput> input) -> void
{
	// Make sure we belong to the same I/O component
	if (&input.get().ioComponent() != &_ioComponent.get())
	{
		/// @todo replace "template I/O point", "I/O batch", and "I/O component" with more descriptive names
		throw std::runtime_error("Attempt to attach template I/O point to I/O batch of different I/O component");
	}

	// Add it
	_inputs.push_back(input);
}

auto TemplateIoBatch::addOutput(std::reference_wrapper<AbstractOutput> output) -> void
{
	// Make sure we belong to the same I/O component
	if (&output.get().ioComponent() != &_ioComponent.get())
	{
		/// @todo replace "template I/O point", "I/O batch", and "I/O component" with more descriptive names
		throw std::runtime_error("Attempt to attach template I/O point to I/O batch of different I/O component");
	}

	// Add it
	_outputs.push_back(output);
}

auto TemplateIoBatch::resolveAttribute(std::u16string_view name) -> const model::Attribute *
{
	// Check the read state attributes
	if (auto attribute = _readState.resolveAttribute(name))
	{
		return attribute;
	}
	// Check the write state attributes
	if (auto attribute = _writeState.resolveAttribute(name))
	{
		return attribute;
	}

	/// @todo add any additional attributes this class supports, including attributes inherited from the I/O component

	return nullptr;
}

auto TemplateIoBatch::resolveTask(std::u16string_view name) -> std::shared_ptr<process::Task>
{
	if (name == u"read"sv)
	{
		return std::shared_ptr<process::Task>(sharedFromThis(), &_readTask);
	}
	if (name == u"write"sv)
	{
		return std::shared_ptr<process::Task>(sharedFromThis(), &_writeTask);
	}

	/// @todo add any additional tasks this class supports

	return nullptr;
}

auto TemplateIoBatch::resolveEvent(std::u16string_view name) -> std::shared_ptr<process::Event>
{
	// Check the read state events
	if (auto event = _readState.resolveEvent(name, sharedFromThis()))
	{
		return event;
	}
	// Check the write state events
	if (auto event = _writeState.resolveEvent(name, sharedFromThis()))
	{
		return event;
	}

	/// @todo add any additional events this class supports, including events inherited from the I/O component

	return nullptr;
}

auto TemplateIoBatch::readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle
{
	// Check the read state attributes
	if (auto handle = _readState.readHandle(_readDataBlock, attribute))
	{
		return *handle;
	}
	// Check the write state attributes
	if (auto handle = _writeState.readHandle(_writeDataBlock, attribute))
	{
		return *handle;
	}

	/// @todo add any additional readable attributes this class supports, including attributes inherited from the I/O component

	return data::ReadHandle::Error::Unknown;
}

auto TemplateIoBatch::resolveReadStateAttribute(std::u16string_view name) -> const model::Attribute *
{
	return _readState.resolveAttribute(name);
}

auto TemplateIoBatch::resolveReadStateEvent(std::u16string_view name) -> std::shared_ptr<process::Event>
{
	return _readState.resolveEvent(name, sharedFromThis());
}

auto TemplateIoBatch::readStateReadHandle(const model::Attribute &attribute) const noexcept
	-> std::optional<data::ReadHandle>
{
	return _readState.readHandle(_readDataBlock, attribute);
}

auto TemplateIoBatch::realize() -> void
{
	// Track the buffer size we need for pending events
	std::size_t readEventCount { 0 };
	std::size_t writeEventCount { 0 };

	// Add our own states
	_readState.attach(_readDataArray, readEventCount);
	_writeState.attach(_writeDataArray, writeEventCount);

	// Attach all the inputs
	for (auto &&input : _inputs)
	{
		input.get().attachInput(_readDataArray, readEventCount);
	}
	// Attach all the outputs
	for (auto &&output : _outputs)
	{
		output.get().attachOutput(_writeDataArray, writeEventCount);
	}

	// Create the data blocks
	_readDataBlock.create(memory::memoryResources::data());
	_writeDataBlock.create(memory::memoryResources::data());

	// Reserve space in the buffers
	_runtimeBuffers._eventsToFire.reset(std::max(readEventCount, writeEventCount));
	_runtimeBuffers._outputsToNotify.reset(_outputs.size());
}

auto TemplateIoBatch::prepare() -> void
{
	// Create a read command
	/// @todo initialized the read command properly based on the inputs to read.
	_readCommand.reset(new ReadCommand);

	/// @todo provide the information needed to decode the value to the inputs, like e.g. the correct data data offsets.
}

auto TemplateIoBatch::ioComponentStateChanged(std::chrono::system_clock::time_point timeStamp, std::error_code error) -> void
{
	// We cannot reset the error to Ok because we don't have a read command payload. So we use the special custom error code instead.
	auto effectiveError = error ? error : CustomError::NoData;

	// Update the inputs. We do not notify the I/O component, because that is who this message comes from in the first place.
	// Note: the write state is not updated, because the write state simply contains the last write error, which is unaffected
	// by I/O component errors.
	updateInputs(timeStamp, effectiveError);
}

auto TemplateIoBatch::performReadTask(const process::ExecutionContext &context) -> void
{
	// Only perform the read only if the I/O component is connected
	if (!_ioComponent.get().connected())
	{
		return;
	}

	// Read the data
	read(context.scheduledTime());
}

auto TemplateIoBatch::read(std::chrono::system_clock::time_point timeStamp) -> void
{
	try
	{
		/// @todo send the read command
		ReadCommand::Payload payload = {};

		/// @todo if the read function does not throw errors, but uses return types or internal handle state,
		// throw an std::system_error here on failure, or call handleReadError() directly.

		// The read was successful
		updateInputs(timeStamp, payload);
	}
	catch (const std::exception &)
	{
		// Get the error from the current exception using this special utility function
		const auto error = utils::eh::currentErrorCode();
		// Handle the error
		handleReadError(timeStamp, error);
	}
}

auto TemplateIoBatch::handleReadError(std::chrono::system_clock::time_point timeStamp, std::error_code error)
	-> void
{
	// Update our own state together with those of the inputs
	updateInputs(timeStamp, error);
	// Notify the I/O component
	_ioComponent.get().handleError(timeStamp, error, this);
}

auto TemplateIoBatch::performWriteTask(const process::ExecutionContext &context) -> void
{
	// Only perform the read only if the I/O component is connected
	if (!_ioComponent.get().connected())
	{
		return;
	}

	// Write the data
	write(context.scheduledTime());
}

auto TemplateIoBatch::write(std::chrono::system_clock::time_point timeStamp) -> void
{
	// Protect use of the list of outputs to notify
	RuntimeBufferSentinel eventsToFireSentinel(_runtimeBuffers._outputsToNotify);

	// Create a command
	WriteCommand command;

	// Collect pending outputs
	for (auto &&output : _outputs)
	{
		// Add the output
		if (output.get().addToWriteCommand(command))
		{
			_runtimeBuffers._outputsToNotify.push_back(output);
		}
	}

	// If there were no pending outputs, just bail
	if (_runtimeBuffers._outputsToNotify.empty())
	{
		return;
	}

	try
	{
		/// @todo send the command

		/// @todo if the write function does not throw errors, but uses return types or internal handle state,
		// throw an std::system_error here on failure, or call handleWriteError() directly.

		// The write was successful
		updateOutputs(timeStamp, std::error_code(), _runtimeBuffers._outputsToNotify);
	}
	catch (const std::exception &)
	{
		// Get the error from the current exception using this special utility function
		const auto error = utils::eh::currentErrorCode();
		// Handle the error
		handleWriteError(timeStamp, error, _runtimeBuffers._outputsToNotify);
	}
}

auto TemplateIoBatch::handleWriteError(std::chrono::system_clock::time_point timeStamp, std::error_code error, const OutputList &outputs)
	-> void
{
	// Update our own state together with those of the inputs
	updateOutputs(timeStamp, error, outputs);
	// Notify the I/O component
	_ioComponent.get().handleError(timeStamp, error, this);
}

auto TemplateIoBatch::updateInputs(std::chrono::system_clock::time_point timeStamp, const utils::eh::Failable<std::reference_wrapper<const ReadCommand::Payload>> &payloadOrError)
	-> void
{
	// Protect use of the pending event buffer
	RuntimeBufferSentinel eventsToFireSentinel(_runtimeBuffers._eventsToFire);

	// Make a write sentinel
	memory::WriteSentinel sentinel { _readDataBlock };

	// Update the common read state
	const auto commonChanges = _readState.update(sentinel, timeStamp, payloadOrError.error(), _runtimeBuffers._eventsToFire);

	// Update all the inputs
	for (auto &&input : _inputs)
	{
		input.get().updateReadState(sentinel, timeStamp, payloadOrError, commonChanges, _runtimeBuffers._eventsToFire);
	}

	// Commit the data before sending the events
	sentinel.commit();

	// Fire the collected events
	for (auto &&event : _runtimeBuffers._eventsToFire)
	{
		event.get().fire();
	}
}

auto TemplateIoBatch::updateOutputs(std::chrono::system_clock::time_point timeStamp, std::error_code error, const OutputList &outputs) -> void
{
	// Protect use of the pending event buffer
	RuntimeBufferSentinel eventsToFireSentinel(_runtimeBuffers._eventsToFire);

	// Make a write sentinel
	memory::WriteSentinel sentinel { _readDataBlock };

	// Update the latest state
	_writeState.update(sentinel, timeStamp, error, _runtimeBuffers._eventsToFire);

	// Update all the relevant outputs
	for (auto &&output : outputs)
	{
		output.get().updateWriteState(sentinel, timeStamp, error, _runtimeBuffers._eventsToFire);
	}

	// Commit the data before sending the events
	sentinel.commit();

	// Fire the collected events
	for (auto &&event : _runtimeBuffers._eventsToFire)
	{
		event.get().fire();
	}
}

} // namespace xentara::plugins::templateDriver