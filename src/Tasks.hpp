// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/process/Task.hpp>

/// @brief Contains the Xentara tasks of the driver
namespace xentara::plugins::templateDriver::tasks
{

/// @brief A Xentara task used to read the data points attached to a batch transaction
extern const process::Task::Role kRead;
/// @brief A Xentara task used to write the data points attached to a batch transaction
extern const process::Task::Role kWrite;

} // namespace xentara::plugins::templateDriver::tasks