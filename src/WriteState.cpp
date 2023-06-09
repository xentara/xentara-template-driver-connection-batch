// Copyright (c) embedded ocean GmbH
#include "WriteState.hpp"

#include "Attributes.hpp"
#include "Events.hpp"

#include <xentara/memory/WriteSentinel.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

auto WriteState::forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool
{
	// Handle all the attributes we support
	return
		function(model::Attribute::kWriteTime) ||
		function(attributes::kWriteError);
}

auto WriteState::forEachEvent(const model::ForEachEventFunction &function, std::shared_ptr<void> parent) -> bool
{
	// Handle all the events we support
	return
		function(events::kWritten, std::shared_ptr<process::Event>(parent, &_writtenEvent)) ||
		function(attributes::kWriteError, std::shared_ptr<process::Event>(parent, &_writeErrorEvent));
}

auto WriteState::makeReadHandle(const DataBlock &dataBlock, const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
{
	// Try each readable attribute
	if (attribute == model::Attribute::kWriteTime)
	{
		return dataBlock.member(_stateHandle, &State::_writeTime);
	}
	else if (attribute == attributes::kWriteError)
	{
		return dataBlock.member(_stateHandle, &State::_writeError);
	}

	return std::nullopt;
}

auto WriteState::attach(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	// Add the state to the array
	_stateHandle = dataArray.appendObject<State>();

	// Add the number of events that can be triggered at once.
	// This is only one, not two, because _writtenEvent and _writeErrorEvent are mutually exclusive
	eventCount += 1;
}

auto WriteState::update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> void
{
	// Get the correct array entry
	auto &state = writeSentinel[_stateHandle];

	// Update the state
	state._writeTime = timeStamp;
	state._writeError = error;

	// Cause the correct events to be fired
	if (!error)
	{
		eventsToFire.push_back(_writtenEvent);
	}
	else
	{
		eventsToFire.push_back(_writeErrorEvent);
	}
}

} // namespace xentara::plugins::templateDriver