// Copyright (c) embedded ocean GmbH
#pragma once

#include "Attributes.hpp"
#include "CustomError.hpp"
#include "Types.hpp"

#include <xentara/data/Quality.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/memory/Array.hpp>
#include <xentara/model/ForEachAttributeFunction.hpp>
#include <xentara/model/ForEachEventFunction.hpp>
#include <xentara/process/Event.hpp>

#include <chrono>
#include <concepts>
#include <optional>
#include <memory>

namespace xentara::plugins::templateDriver
{

/// @brief Common state information for a read operation.
///
/// This class contains all the read state information that is common to all values read using the same transaction.
class CommonReadState final
{
public:
	/// @brief Changes that may occur when updating the data
	struct Changes
	{
		/// @brief Whether the quality has changed
		bool _qualityChanged { false };
		/// @brief Whether the error changed
		bool _errorChanged { false };

		/// @brief Determine whether anything changed
		constexpr explicit operator bool() const noexcept
		{
			return _qualityChanged || _errorChanged;
		}
	};

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

	/// @brief Attaches the state to its I/O transaction
	/// @param dataArray The data array that the attributes should be added to. The caller will use the information in this array
	/// to allocate the data block.
	/// @param eventCount A variable that counts the total number of events than can be raised for a single update.
	/// The maximum number of events that update() will request to be raised will be added to this variable. The caller will use this
	/// event count to preallocate a buffer when collecting the events to raise after an update.
	auto attach(memory::Array &dataArray, std::size_t &eventCount) -> void;

	/// @brief Updates the data and collects the events to send
	/// @param writeSentinel A write sentinel for the data block the data is stored in
	/// @param timeStamp The update time stamp
	/// @param error The error code, or a default constructed std::error_code object to reset the error
	/// @param eventsToRaise Any events that need to be raised as a result of the update will be added to this
	/// list. The events will not be raised directly, because the write sentinel needs to be commited first,
	/// which is done by the caller.
	/// @return An object conmtaining information about which parts of the state changed, if any.
	auto update(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		std::error_code error,
		PendingEventList &eventsToRaise) -> Changes;

private:
	/// @brief This structure is used to represent the state inside the memory block
	struct State final
	{
		/// @brief The update time stamp
		std::chrono::system_clock::time_point _updateTime { std::chrono::system_clock::time_point::min() };
		/// @brief The quality of the value
		data::Quality _quality { data::Quality::Bad };
		/// @brief The error code when reading the value, or a default constructed std::error_code object for none.
		std::error_code _error { CustomError::NotConnected };
	};

	/// @brief A Xentara event that is raised when the inputs were read (sucessfully or not)
	process::Event _readEvent { io::Direction::Input };

	/// @brief The array element that contains the state
	memory::Array::ObjectHandle<State> _stateHandle;
};

} // namespace xentara::plugins::templateDriver