// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/utils/tools/Unique.hpp>

namespace xentara::plugins::templateDriver
{

/// @brief A command used to read inputs
/// @todo implement a proper read command
class ReadCommand final : private utils::tools::Unique
{
public:
	/// @brief Some class that represents the data received from the device
	/// @todo use a suitable class to represent the data
	class Payload final
	{
	};
};

} // namespace xentara::plugins::templateDriver