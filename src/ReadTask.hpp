// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/process/Task.hpp>
#include <xentara/process/ExecutionContext.hpp>

#include <chrono>
#include <functional>

namespace xentara::plugins::templateDriver
{

/// @brief This class providing callbacks for the Xentara scheduler for the "read" task of I/O points
template <typename Target>
class ReadTask final : public process::Task
{
public:
	/// @brief This constuctor attached the task to its target
	ReadTask(std::reference_wrapper<Target> target) : _target(target)
	{
	}

	/// @name Virtual Overrides for process::Task
	/// @{

	auto stages() const -> Stages final
	{
		return Stage::PreOperational | Stage::Operational | Stage::PostOperational;
	}

	auto preparePreOperational(const process::ExecutionContext &context) -> Status final;

	auto preOperational(const process::ExecutionContext &context) -> Status final;

	auto operational(const process::ExecutionContext &context) -> void final;

	auto preparePostOperational(const process::ExecutionContext &context) -> Status final;
		
	/// @}

private:
	/// @brief A reference to the target element
	std::reference_wrapper<Target> _target;
};

template <typename Target>
auto ReadTask<Target>::preparePreOperational(const process::ExecutionContext &context) -> Status
{
	// Request a connection
	_target.get().requestConnect(context.scheduledTime());

	// Read the value once to initialize it
	operational(context);

	// We are done now. Even if we couldn't read the value, we proceed to the next stage,
	// because attempting again is unlikely to succeed any better.
	return Status::Ready;
}

template <typename Target>
auto ReadTask<Target>::preOperational(const process::ExecutionContext &context) -> Status
{
	// We just do the same thing as in the operational stage
	operational(context);

	return Status::Ready;
}

template <typename Target>
auto ReadTask<Target>::operational(const process::ExecutionContext &context) -> void
{
	_target.get().performReadTask(context);
}

template <typename Target>
auto ReadTask<Target>::preparePostOperational(const process::ExecutionContext &context) -> Status
{
	// Request a disconnect
	_target.get().requestDisconnect(context.scheduledTime());

	return Status::Completed;
}

} // namespace xentara::plugins::templateDriver