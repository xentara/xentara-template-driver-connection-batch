// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/utils/tools/Unique.hpp>

namespace xentara::plugins::templateDriver
{

/// @brief A command used to write outputs
/// @todo implement a proper write command
class WriteCommand final : private utils::tools::Unique
{
};

} // namespace xentara::plugins::templateDriver