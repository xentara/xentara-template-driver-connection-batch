// Copyright (c) embedded ocean GmbH
#pragma once

#include "CustomError.hpp"

#include <xentara/model/Attribute.hpp>

#include <cstdint>
#include <system_error>

/// @brief Contains the Xentara attributes of the driver
namespace xentara::plugins::templateDriver::attributes
{

/// @brief A Xentara attribute containing a read error code for an I/O point
extern const model::Attribute kError;
/// @brief A Xentara attribute containing a write error code for an I/O point
extern const model::Attribute kWriteError;

/// @brief A Xentara attribute containing the connection time for an I/O component
extern const model::Attribute kConnectionTime;
/// @brief A Xentara attribute containing an error code for an I/O component
extern const model::Attribute kDeviceError;

} // namespace xentara::plugins::templateDriver::attributes