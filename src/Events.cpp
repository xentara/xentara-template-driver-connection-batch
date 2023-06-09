// Copyright (c) embedded ocean GmbH
#include "Events.hpp"

#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver::events
{

using namespace std::literals;
using namespace xentara::literals;

/// @todo assign a unique UUID
const process::Event::Role kRead { "deadbeef-dead-beef-dead-beefdeadbeef"_uuid, "read"sv };

/// @todo assign a unique UUID
const process::Event::Role kWritten { "deadbeef-dead-beef-dead-beefdeadbeef"_uuid, "written"sv };

} // namespace xentara::plugins::templateDriver::events