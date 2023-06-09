// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "CommonReadState.hpp"
#include "ReadCommand.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/utils/eh/expected.hpp>

#include <chrono>
#include <system_error>
#include <cstdlib>

namespace xentara::plugins::templateDriver
{

class TemplateIoComponent;

/// @brief Base class for inputs and outputs that can be read by a batch transaction
///
/// This class is used for inputs, but also for outputs. This is the case because outputs need to be able to read back
/// the currently set value from the I/O component.
class AbstractInput
{
public:
	/// @brief Virtual destructor
	/// @note The destructor is pure virtual (= 0) to ensure that this class will remain abstract, even if we should remove all
	/// other pure virtual functions later. This is not necessary, of course, but prevents the abstract class from becoming
	/// instantiable by accident as a result of refactoring.
	virtual ~AbstractInput() = 0;

	/// @brief Gets the I/O component the input belongs to
	/// @todo give this a more descriptive name, e.g. "_device"
	virtual auto ioComponent() const -> const TemplateIoComponent & = 0;
	
	/// @brief Attaches the input to its batch transaction
	/// @param dataArray The data array that the attributes should be added to. The caller will use the information in this array
	/// to allocate the data block.
	/// @param eventCount A variable that counts the total number of events than can be fired for a single update.
	/// The maximum number of events that update() will request to be fired will be added to this variable. The caller will use this
	/// event count to preallocate a buffer when collecting the events to fire after an update.
	virtual auto attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void = 0;

	/// @brief Updates the read state and collects the events to send
	/// @param writeSentinel A write sentinel for the data block the data is stored in
	/// @param timeStamp The update time stamp
	/// @param payloadOrError This is a variant-like type that will hold either the payload of the read command, or an std::error_code object
	/// containing a read error.
	/// @param commonChanges An object containing information about which parts of the common read state changed, if any.
	/// @param eventsToFire Any events that need to be fired as a result of the update will be added to this
	/// list. The events will not be fired directly, because the write sentinel needs to be commited first,
	/// which is done by the caller.
	virtual auto updateReadState(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::expected<std::reference_wrapper<const ReadCommand::Payload>, std::error_code> &payloadOrError,
		const CommonReadState::Changes &commonChanges,
		PendingEventList &eventsToFire) -> void = 0;
};

inline AbstractInput::~AbstractInput() = default;

} // namespace xentara::plugins::templateDriver