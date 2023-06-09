// Copyright (c) embedded ocean GmbH
#include "CommonReadState.hpp"

#include "Attributes.hpp"
#include "Events.hpp"

#include <xentara/memory/WriteSentinel.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

auto CommonReadState::forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool
{
	// Handle all the attributes we support
	return
		function(model::Attribute::kUpdateTime) ||
		function(model::Attribute::kQuality) ||
		function(attributes::kError);
}

auto CommonReadState::forEachEvent(const model::ForEachEventFunction &function, std::shared_ptr<void> parent) -> bool
{
	// Handle all the events we support
	return
		function(events::kRead, std::shared_ptr<process::Event>(parent, &_readEvent)) ||
		function(model::Attribute::kQuality, std::shared_ptr<process::Event>(parent, &_qualityChangedEvent));
}

auto CommonReadState::makeReadHandle(const DataBlock &dataBlock,
	const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
{
	// Try each readable attribute
	if (attribute == model::Attribute::kUpdateTime)
	{
		return dataBlock.member(_stateHandle, &State::_updateTime);
	}
	else if (attribute == model::Attribute::kQuality)
	{
		return dataBlock.member(_stateHandle, &State::_quality);
	}
	else if (attribute == attributes::kError)
	{
		return dataBlock.member(_stateHandle, &State::_error);
	}

	return std::nullopt;
}

auto CommonReadState::attach(memory::Array &dataArray, std::size_t &eventCount) -> void
{
	// Add the state to the array
	_stateHandle = dataArray.appendObject<State>();

	// Add the number of events that can be triggered at once, which is all of them.
	eventCount += 2;
}

auto CommonReadState::update(
		WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> Changes
{
	// Get the correct array entry
	auto &state = writeSentinel[_stateHandle];
	const auto &oldState = writeSentinel.oldValues()[_stateHandle];

	state._updateTime = timeStamp;

	// See if the operation was a success
	if (!error)
	{
		// Reset the error
		state._quality = data::Quality::Good;
		state._error = {};
	}
	// We have an error
	else
	{
		// Set the error
		state._quality = data::Quality::Bad;
		state._error = error;
	}

	// Detect changes
	const Changes changes {
		._qualityChanged = state._quality != oldState._quality,
		._errorChanged = state._error != oldState._error };

	// Cause the correct events to be fired
	if (error != CustomError::NoData)
	{
		eventsToFire.push_back(_readEvent);
	}
	if (changes._qualityChanged)
	{
		eventsToFire.push_back(_qualityChangedEvent);
	}

	return changes;
}

} // namespace xentara::plugins::templateDriver