// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/process/Task.hpp>
#include <xentara/process/ExecutionContext.hpp>

#include <chrono>
#include <functional>

namespace xentara::plugins::templateDriver
{

/// @brief This class providing callbacks for the Xentara scheduler for the "write" task of batch transactions
template <typename Target>
class WriteTask final : public process::Task
{
public:
	/// @brief This constuctor attached the task to its target
	WriteTask(std::reference_wrapper<Target> target) : _target(target)
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
auto WriteTask<Target>::preparePreOperational(const process::ExecutionContext &context) -> Status
{
	// Request a connection
	_target.get().requestConnect(context.scheduledTime());

	return Status::Ready;
}

template <typename Target>
auto WriteTask<Target>::preOperational(const process::ExecutionContext &context) -> Status
{
	// We just do the same thing as in the operational stage
	operational(context);

	return Status::Ready;
}

template <typename Target>
auto WriteTask<Target>::operational(const process::ExecutionContext &context) -> void
{
	_target.get().performWriteTask(context);
}

template <typename Target>
auto WriteTask<Target>::preparePostOperational(const process::ExecutionContext &context) -> Status
{
	// Request a disconnect
	_target.get().requestDisconnect(context.scheduledTime());

	return Status::Completed;
}

} // namespace xentara::plugins::templateDriver