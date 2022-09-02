// Copyright (c) embedded ocean GmbH
#pragma once

#include "Driver.hpp"

#include <xentara/plugin/Plugin.hpp>

/// @namespace xentara
/// @brief base namespace for Xentara

/// @namespace xentara::plugins
/// @brief base namespace for Xentara plugins

/// @brief base namespace for the driver
/// @todo rename namespace
namespace xentara::plugins::templateDriver
{

/// @brief This is the plugin class.
///
/// This class registers all the drivers and service providers.
class Plugin final : plugin::Plugin
{
public:
	auto registerObjects(Registry & registry) -> void final
	{
		// Register the driver object.
		registry << _driver;
	}

private:
	/// @brief The driver object
	Driver _driver;

	/// @brief The global plugin object
	static Plugin _instance;
};

} // namespace xentara::plugins::templateDriver
