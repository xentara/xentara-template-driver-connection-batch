// Copyright (c) embedded ocean GmbH
#pragma once

#include "TemplateIoComponent.hpp"
#include "Attributes.hpp"
#include "CommonReadState.hpp"
#include "WriteState.hpp"
#include "CustomError.hpp"
#include "Types.hpp"
#include "ReadCommand.hpp"
#include "ReadTask.hpp"
#include "WriteTask.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/skill/Element.hpp>
#include <xentara/skill/EnableSharedFromThis.hpp>
#include <xentara/utils/core/Uuid.hpp>
#include <xentara/utils/eh/expected.hpp>

#include <string_view>
#include <functional>
#include <memory>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

class AbstractInput;
class AbstractOutput;

/// @brief A class representing a specific type of batch transaction.
/// @todo rename this class to something more descriptive
class TemplateBatchTransaction final : public skill::Element, public TemplateIoComponent::ErrorSink, public skill::EnableSharedFromThis<TemplateBatchTransaction>
{
public:
	/// @brief The class object containing meta-information about this element type
	/// @todo change class name
	/// @todo assign a unique UUID
	/// @todo change display name
	using Class = ConcreteClass<"TemplateBatchTransaction",
		"deadbeef-dead-beef-dead-beefdeadbeef"_uuid,
		"template driver transaction">;

	/// @brief This constructor attaches the batch to its I/O component
	TemplateBatchTransaction(std::reference_wrapper<TemplateIoComponent> ioComponent) :
		_ioComponent(ioComponent)
	{
		ioComponent.get().addErrorSink(*this);
	}
	
	/// @brief Adds an input to be processed by the batch
	auto addInput(std::reference_wrapper<AbstractInput> input) -> void;

	/// @brief Iterates over all the attributes that belong to the common read state.
	/// @param function The function that should be called for each attribute
	/// @return The return value of the last function call
	auto forEachReadStateAttribute(const model::ForEachAttributeFunction &function) const -> bool;

	/// @brief Iterates over all the events that belong to the common read state.
	/// @param function The function that should be called for each event
	/// @return The return value of the last function call
	auto forEachReadStateEvent(const model::ForEachEventFunction &function) -> bool;

	/// @brief Creates a read-handle for an attribute that belong to the common read state.
	/// @param attribute The attribute to create the handle for
	/// @return A read handle for the attribute, or std::nullopt if the read state doesn't know the attribute
	auto makeReadStateReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>;

	/// @brief Gets the data block that holds the data for the read operations
	constexpr auto readDataBlock() noexcept -> DataBlock &
	{
		return _readDataBlock;
	}
	/// @overload 
	constexpr auto readDataBlock() const noexcept -> const DataBlock &
	{
		return _readDataBlock;
	}
	
	/// @brief This function adds an output to be processed by the batch
	auto addOutput(std::reference_wrapper<AbstractOutput> output) -> void;

	/// @brief Gets the data block that holds the data for the write operations
	constexpr auto writeDataBlock() noexcept -> DataBlock &
	{
		return _writeDataBlock;
	}
	/// @overload 
	constexpr auto writeDataBlock() const noexcept -> const DataBlock &
	{
		return _writeDataBlock;
	}
	
	/// @name Virtual Overrides for skill::Element
	/// @{

	auto forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool final;
	
	auto forEachEvent(const model::ForEachEventFunction &function) -> bool final;

	auto forEachTask(const model::ForEachTaskFunction &function) -> bool final;

	auto makeReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle> final;

	auto realize() -> void final;

	auto prepare() -> void final;

	/// @}

	/// @name Virtual Overrides for TemplateIoComponent::ErrorSink
	/// @{

	auto ioComponentStateChanged(std::chrono::system_clock::time_point timeStamp, std::error_code error) -> void final;

	/// @}

private:
	// The tasks need access to out private member functions
	friend class ReadTask<TemplateBatchTransaction>;
	friend class WriteTask<TemplateBatchTransaction>;

	/// @brief This function is forwarded to the I/O component.
	auto requestConnect(std::chrono::system_clock::time_point timeStamp) noexcept -> void
	{
		_ioComponent.get().requestConnect(timeStamp);
	}

	/// @brief This function is forwarded to the I/O component.
	auto requestDisconnect(std::chrono::system_clock::time_point timeStamp) noexcept -> void
	{
		_ioComponent.get().requestDisconnect(timeStamp);
	}

	/// @brief This function is called by the "read" task.
	///
	/// This function attempts to read the value if the I/O component is up.
	auto performReadTask(const process::ExecutionContext &context) -> void;
	/// @brief Attempts to read the data from the I/O component and updates the state accordingly.
	auto read(std::chrono::system_clock::time_point timeStamp) -> void;
	/// @brief Handles a read error
	auto handleReadError(std::chrono::system_clock::time_point timeStamp, std::error_code error) -> void;

	/// @brief This function is called by the "write" task.
	///
	/// This function attempts to write the value if the I/O component is up.
	auto performWriteTask(const process::ExecutionContext &context) -> void;
	/// @brief Attempts to write any pending value to the I/O component and updates the state accordingly.
	auto write(std::chrono::system_clock::time_point timeStamp) -> void;	
	/// @brief Handles a write error
	auto handleWriteError(std::chrono::system_clock::time_point timeStamp, std::error_code error, const OutputList &outputs) -> void;

	/// @brief Updates the inputs with valid data and sends events
	/// @param timeStamp The update time stamp
	/// @param payloadOrError This is a variant-like type that will hold either the payload of the read command, or an std::error_code object
	/// containing a read error.
	auto updateInputs(std::chrono::system_clock::time_point timeStamp,
		const utils::eh::expected<std::reference_wrapper<const ReadCommand::Payload>, std::error_code> &payloadOrError) -> void;

	/// @brief Updates the outputs and sends events
	/// @param timeStamp The update time stamp
	/// @param error The error code, or a default constructed std::error_code object if no error occurred
	/// @param outputs The outputs to update
	auto updateOutputs(
		std::chrono::system_clock::time_point timeStamp, std::error_code error, const OutputList &outputs) -> void;

	/// @brief The I/O component this batch belongs to
	/// @todo give this a more descriptive name, e.g. "_device"
	std::reference_wrapper<TemplateIoComponent> _ioComponent;

	/// @class xentara::plugins::templateDriver::TemplateBatchTransaction
	/// @todo Split read and write command split into several commands each, if necessary.
	/// 
	/// @todo Some Some I/O components may need to have the read and write command split into several commands each.
	/// Some I/O components may require a separate command for each data type, for example, or may only be able to
	/// read or write objects with continuous addresses. if this is the case, each separate command needs its own list
	/// of inputs and/or output, as well as its own read state and write state.

	/// @name Virtual Overrides for skill::Element
	/// @{

	auto load(utils::json::decoder::Object &jsonObject, config::Context &context) -> void final;

	/// @}

	/// @brief The list of inputs
	std::list<std::reference_wrapper<AbstractInput>> _inputs;
	/// @brief The list of outputs
	std::list<std::reference_wrapper<AbstractOutput>> _outputs;

	/// @brief The read command to send, or nullptr if it hasn't been constructed yet.
	std::unique_ptr<ReadCommand> _readCommand;

	/// @class xentara::plugins::templateDriver::TemplateBatchTransaction
	/// @note There is no member for the write command, as the write command is constructed on-the-fly,
	/// depending on which outputs wave to be written.

	/// @brief The array that describes the structure of the read data block
	memory::Array _readDataArray;
	/// @brief The data block that holds the data for the inputs
	DataBlock _readDataBlock { _readDataArray };

	/// @brief The array that describes the structure of the write data block
	memory::Array _writeDataArray;
	/// @brief The data block that holds the data for the outputs
	DataBlock _writeDataBlock { _writeDataArray };

	/// @brief The common read state for all inputs
	CommonReadState _readState;
	/// @brief The state for the last write command 
	WriteState _writeState;

	/// @brief The "read" task
	ReadTask<TemplateBatchTransaction> _readTask { *this };
	/// @brief The "write" task
	WriteTask<TemplateBatchTransaction> _writeTask { *this };

	/// @brief Preallocated runtime buffers
	///
	/// This structure contains preallocated buffers for data needed when sending commands.
	/// the buffers are preallocated to avoid memory allocations in the read() and write() functions,
	/// which would not be real-time safe.
	struct
	{
		/// @brief The list of events to raise after a read or write
		PendingEventList _eventsToRaise;

		/// @brief The outputs to notify after a write operation
		OutputList _outputsToNotify;
	} _runtimeBuffers;

	/// @class xentara::plugins::templateDriver::TemplateBatchTransaction::RuntimeBufferSentinel
	/// @brief A sentinel that performs initialization and cleanup of a runtime buffer
	template <typename Buffer>
	class RuntimeBufferSentinel;
};

} // namespace xentara::plugins::templateDriver