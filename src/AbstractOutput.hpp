// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "CommonReadState.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>

#include <chrono>
#include <system_error>
#include <cstdlib>

namespace xentara::plugins::templateDriver
{

class WriteCommand;

/// @brief Base class for outputs that can be written by a batch transaction
class AbstractOutput
{
public:
	/// @brief Virtual destructor
	/// @note The destructor is pure virtual (= 0) to ensure that this class will remain abstract, even if we should remove all
	/// other pure virtual functions later. This is not necessary, of course, but prevents the abstract class from becoming
	/// instantiable by accident as a result of refactoring.
	virtual ~AbstractOutput() = 0;

	/// @brief Gets the I/O component the output belongs to
	/// @todo give this a more descriptive name, e.g. "_device"
	virtual auto ioComponent() const -> const TemplateIoComponent & = 0;
		
	/// @brief Attaches the output to its batch transaction
	/// @param dataArray The data array that the attributes should be added to. The caller will use the information in this array
	/// to allocate the data block.
	/// @param eventCount A variable that counts the total number of events than can be raised for a single update.
	/// The maximum number of events that update() will request to be raised will be added to this variable. The caller will use this
	/// event count to preallocate a buffer when collecting the events to raise after an update.
	virtual auto attachOutput(memory::Array &dataArray, std::size_t &eventCount) -> void = 0;

	/// @brief Adds any pending output value to a write command.
	/// @param command The write command to add the value to.
	/// @return This function must return *true* if data was added, or *false* if no value was pending.
	virtual auto addToWriteCommand(WriteCommand &command) -> bool = 0;

	/// @brief Updates the write state and collects the events to send
	/// @param writeSentinel A write sentinel for the data block the data is stored in
	/// @param timeStamp The update time stamp
	/// @param error The error code, or a default constructed std::error_code object if no error occurred
	/// @param eventsToRaise Any events that need to be raised as a result of the update will be added to this
	/// list. The events will not be raised directly, because the write sentinel needs to be commited first,
	/// which is done by the caller.
	virtual auto updateWriteState(
		WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToRaise) -> void = 0;
};

inline AbstractOutput::~AbstractOutput() = default;

} // namespace xentara::plugins::templateDriver