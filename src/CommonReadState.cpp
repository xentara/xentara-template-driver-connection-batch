// Copyright (c) embedded ocean GmbH
#include "CommonReadState.hpp"

#include "Attributes.hpp"

#include <xentara/memory/WriteSentinel.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

auto CommonReadState::resolveAttribute(std::string_view name) -> const model::Attribute *
{
	// Check all the attributes we support
	return model::Attribute::resolve(name,
		model::Attribute::kUpdateTime,
		model::Attribute::kQuality,
		attributes::kError);
}

auto CommonReadState::resolveEvent(std::string_view name, std::shared_ptr<void> parent) -> std::shared_ptr<process::Event>
{
	// Check all the events we support
	if (name == "read"sv)
	{
		return std::shared_ptr<process::Event>(parent, &_readEvent);
	}
	else if (name == model::Attribute::kQuality)
	{
		return std::shared_ptr<process::Event>(parent, &_qualityChangedEvent);
	}

	// The event name is not known
	return nullptr;
}

auto CommonReadState::readHandle(const DataBlock &dataBlock,
	const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
{
	// Try reach readable attribute
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