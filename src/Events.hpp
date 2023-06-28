// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/process/Event.hpp>

/// @brief Contains the Xentara events of the driver
namespace xentara::plugins::templateDriver::events
{

/// @brief A Xentara event that is raised when a data point was read
extern const process::Event::Role kRead;
/// @brief A Xentara event that is raised when a data point was written
extern const process::Event::Role kWritten;

} // namespace xentara::plugins::templateDriver::events