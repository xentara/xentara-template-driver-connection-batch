// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/utils/atomic/Optional.hpp>

namespace xentara::plugins::templateDriver
{

/// @brief A thread-safe, lock-free queue that can hold a single value.
///
/// This queues only allows enqueuing a single value. Enqueuing a second value will overwrite the first.
template <typename DataType>
class SingleValueQueue final
{
public:
	/// @brief Enqueues a value.
	/// 
	/// Any value already in the queue will be replaced.
	/// @param value The value to place in the queue
	auto enqueue(const DataType &value) noexcept -> void
	{
		_value.store(value, std::memory_order_release);
	}

	/// @brief Gets the last scheduled value and removes it from the queue
	/// @return The scheduled value or std::nullopt if none was scheduled since the last call
	auto dequeue() noexcept -> std::optional<DataType>
	{
		return _value.exchange(std::nullopt, std::memory_order_acq_rel);
	}

private:
	/// @brief The queued value, or std::nullopt if the queue is empty.
	utils::atomic::Optional<DataType> _value;

	// Check that the value is lock free, or blocking will occurr
	static_assert(decltype(_value)::is_always_lock_free);
};

} // namespace xentara::plugins::templateDriver