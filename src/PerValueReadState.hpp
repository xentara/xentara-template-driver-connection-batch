// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "Attributes.hpp"
#include "CommonReadState.hpp"

#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/ForEachAttributeFunction.hpp>
#include <xentara/model/ForEachEventFunction.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/utils/eh/expected.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <memory>

namespace xentara::plugins::templateDriver
{

/// @brief Per-value state information for a read operation.
/// 
/// This class contains all the read state information that is tracked separately for all the values read as a batch.
template <std::regular DataType>
class PerValueReadState final
{
public:
	/// @brief Iterates over all the attributes that belong to this state.
	/// @param function The function that should be called for each attribute
	/// @return The return value of the last function call
	auto forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool;

	/// @brief Iterates over all the events that belong to this state.
	/// @param function The function that should be called for each events
	/// @param parent
	/// @parblock
	/// A shared pointer to the containing object.
	/// 
	/// The pointer is used in the aliasing constructor of std::shared_ptr when constructing the event pointers,
	/// so that they will share ownership information with pointers to the parent object.
	/// @endparblock
	/// @return The return value of the last function call
	auto forEachEvent(const model::ForEachEventFunction &function, std::shared_ptr<void> parent) -> bool;

	/// @brief Creates a read-handle for an attribute that belong to this state.
	/// @note The value attribute is not handled, it must be gotten separately using valueReadHandle().
	/// @param dataBlock The data block the data is stored in
	/// @param attribute The attribute to create the handle for
	/// @return A read handle for the attribute, or std::nullopt if the attribute is unknown (including the value attribute)
	auto makeReadHandle(const DataBlock &dataBlock, const model::Attribute &attribute) const noexcept
		-> std::optional<data::ReadHandle>;

	/// @brief Creates a read-handle for the value attribute
	/// @param dataBlock The data block the data is stored in
	/// @return A read handle to the value attribute
	auto valueReadHandle(const DataBlock &dataBlock) const noexcept -> data::ReadHandle;

	/// @brief Attaches the state to its batch transaction
	/// @param dataArray The data array that the attributes should be added to. The caller will use the information in this array
	/// to allocate the data block.
	/// @param eventCount A variable that counts the total number of events than can be fired for a single update.
	/// The maximum number of events that update() will request to be fired will be added to this variable. The caller will use this
	/// event count to preallocate a buffer when collecting the events to fire after an update.
	auto attach(memory::Array &dataArray, std::size_t &eventCount) -> void;

	/// @brief Updates the data and collects the events to send
	/// @param writeSentinel A write sentinel for the data block the data is stored in
	/// @param timeStamp The update time stamp
	/// @param valueOrError This is a variant-like type that will hold either the new value, or an std::error_code object
	/// containing a read error.
	/// @param commonChanges An object containing information about which parts of the common read state changed, if any.
	/// @param eventsToFire Any events that need to be fired as a result of the update will be added to this
	/// list. The events will not be fired directly, because the write sentinel needs to be commited first,
	/// which is done by the caller.
	auto update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::expected<DataType, std::error_code> &valueOrError,
		const CommonReadState::Changes &commonChanges,
		PendingEventList &eventsToFire) -> void;

private:
	/// @brief This structure is used to represent the state inside the memory block
	struct State final
	{
		/// @brief The current value
		DataType _value {};
		/// @brief The change time stamp
		std::chrono::system_clock::time_point _changeTime { std::chrono::system_clock::time_point::min() };
	};

	/// @brief A summary event that is fired when anything changes
	process::Event _changedEvent { io::Direction::Input };

	/// @brief The array element that contains the state
	memory::Array::ObjectHandle<State> _stateHandle;
};

/// @class xentara::plugins::templateDriver::PerValueReadState
/// @todo add extern template statements for other supported types
extern template class PerValueReadState<double>;

} // namespace xentara::plugins::templateDriver