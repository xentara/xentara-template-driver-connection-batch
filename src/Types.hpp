// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/memory/Array.hpp>
#include <xentara/memory/ArrayBlock.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/process/EventList.hpp>
#include <xentara/utils/core/FixedVector.hpp>

namespace xentara::plugins::templateDriver
{

class AbstractOutput;

/// @brief This is the type used for the data blocks that hold the data for the inputs and the outputs
using DataBlock = memory::ArrayBlock;

/// @brief This is the type used for the data blocks that hold the data for the inputs and the outputs
using WriteSentinel = memory::WriteSentinel<memory::Array>;

/// @brief This is the type used to hold the list of events to trigger
using PendingEventList = process::FixedEventList;

/// @brief This is the type used to hold the list of outputs to notify
using OutputList = utils::core::FixedVector<std::reference_wrapper<AbstractOutput>>;

} // namespace xentara::plugins::templateDriver
