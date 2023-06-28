// Copyright (c) embedded ocean GmbH
#include "PerValueReadState.hpp"

#include "Attributes.hpp"

#include <xentara/memory/WriteSentinel.hpp>

namespace xentara::plugins::templateDriver
{

template <std::regular DataType>
auto PerValueReadState<DataType>::forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool
{
	// Handle all the attributes we support
	return
		function(model::Attribute::kChangeTime);
}

template <std::regular DataType>
auto PerValueReadState<DataType>::forEachEvent(const model::ForEachEventFunction &function, std::shared_ptr<void> parent) -> bool
{
	// Handle all the events we support
	return
		function(process::Event::kChanged, std::shared_ptr<process::Event>(parent, &_changedEvent));
}

template <std::regular DataType>
auto PerValueReadState<DataType>::makeReadHandle(const DataBlock &dataBlock,
	const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
{
	// Try each readable attribute
	if (attribute == model::Attribute::kChangeTime)
	{
		return dataBlock.member(_stateHandle, &State::_changeTime);
	}

	return std::nullopt;
}

template <std::regular DataType>
auto PerValueReadState<DataType>::valueReadHandle(const DataBlock &dataBlock) const noexcept -> data::ReadHandle
{
	return dataBlock.member(_stateHandle, &State::_value);
}

template <std::regular DataType>
auto PerValueReadState<DataType>::attach(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	// Add the state to the array
	_stateHandle = dataArray.appendObject<State>();

	// Add the number of events that can be triggered at once, which is just the one event we have.
	eventCount += 1;
}

template <std::regular DataType>
auto PerValueReadState<DataType>::update(
	WriteSentinel &writeSentinel,
	std::chrono::system_clock::time_point timeStamp,
	const utils::eh::expected<DataType, std::error_code> &valueOrError,
	const CommonReadState::Changes &commonChanges,
	PendingEventList &eventsToFire) -> void
{
	// Get the correct array entry
	auto &state = writeSentinel[_stateHandle];
	const auto &oldState = writeSentinel.oldValues()[_stateHandle];

	// Set the value, replacing errors with a default constructed value
	state._value = valueOrError.value_or(DataType());

	// Detect changes
	const auto valueChanged = state._value != oldState._value;
	const auto changed = valueChanged || commonChanges;

	// Update the change time, if necessary. We always need to write the change time, even if it is the same as before,
	// because memory resources use swap-in.
	state._changeTime = changed ? timeStamp : oldState._changeTime;

	// Cause the correct events to be fired
	if (changed)
	{
		eventsToFire.push_back(_changedEvent);
	}
}

/// @class xentara::plugins::templateDriver::PerValueReadState
/// @todo add template instantiations for other supported types
template class PerValueReadState<double>;

} // namespace xentara::plugins::templateDriver