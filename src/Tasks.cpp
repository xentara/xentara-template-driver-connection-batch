// Copyright (c) embedded ocean GmbH
#include "Tasks.hpp"

#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver::tasks
{

using namespace std::literals;
using namespace xentara::literals;

/// @todo assign a unique UUID
const process::Task::Role kRead { "deadbeef-dead-beef-dead-beefdeadbeef"_uuid, "read"sv };

/// @todo assign a unique UUID
const process::Task::Role kWrite { "deadbeef-dead-beef-dead-beefdeadbeef"_uuid, "write"sv };

} // namespace xentara::plugins::templateDriver::tasks