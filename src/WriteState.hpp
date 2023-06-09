// Copyright (c) embedded ocean GmbH
#pragma once

#include "Types.hpp"
#include "Attributes.hpp"

#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/ForEachAttributeFunction.hpp>
#include <xentara/model/ForEachEventFunction.hpp>
#include <xentara/process/Event.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <memory>

namespace xentara::plugins::templateDriver
{

/// @brief State information for a write operation.
class WriteState final
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
	/// @param dataBlock The data block the data is stored in
	/// @param attribute The attribute to create the handle for
	/// @return A read handle for the attribute, or std::nullopt if the attribute is unknown
	auto makeReadHandle(const DataBlock &dataBlock, const model::Attribute &attribute) const noexcept
		-> std::optional<data::ReadHandle>;

	/// @brief Attaches the state to a batch transaction
	/// @param dataArray The data array that the attributes should be added to. The caller will use the information in this array
	/// to allocate the data block.
	/// @param eventCount A variable that counts the total number of events than can be fired for a single update.
	/// The maximum number of events that update() will request to be fired will be added to this variable. The caller will use this
	/// event count to preallocate a buffer when collecting the events to fire after an update.
	auto attach(memory::Array &dataArray, std::size_t &eventCount) -> void;

	/// @brief Updates the data and collects the events to send
	/// @param writeSentinel A write sentinel for the data block the data is stored in
	/// @param timeStamp The update time stamp
	/// @param error The error code, or a default constructed std::error_code object if no error occurred
	/// @param eventsToFire Any events that need to be fired as a result of the update will be added to this
	/// list. The events will not be fired directly, because the write sentinel needs to be commited first,
	/// which is done by the caller.
	auto update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToFire) -> void;

private:
	/// @brief This structure is used to represent the state inside the memory block
	struct State final
	{
		/// @brief The last time the value was written (successfully or not)
		std::chrono::system_clock::time_point _writeTime { std::chrono::system_clock::time_point::min() };
		/// @brief The error code when writing the value, or a default constructed std::error_code object for none.
		/// @note The error is default initialized, because it is not an error if the value was never written.
		std::error_code _writeError;
	};

	/// @brief A Xentara event that is fired when the value was successfully written
	process::Event _writtenEvent { io::Direction::Output };
	/// @brief A Xentara event that is fired when a write error occurred
	process::Event _writeErrorEvent { io::Direction::Output };

	/// @brief The array element that contains the state
	memory::Array::ObjectHandle<State> _stateHandle;
};

} // namespace xentara::plugins::templateDriver