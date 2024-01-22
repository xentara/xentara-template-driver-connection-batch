// Copyright (c) embedded ocean GmbH
#pragma once

#include "AbstractInput.hpp"
#include "PerValueReadState.hpp"

#include <xentara/skill/DataPoint.hpp>
#include <xentara/skill/EnableSharedFromThis.hpp>

#include <functional>
#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

class TemplateIoComponent;
class TemplateBatchTransaction;

/// @brief A class representing a specific type of input.
/// @todo rename this class to something more descriptive
class TemplateInput final : public skill::DataPoint, public AbstractInput, public skill::EnableSharedFromThis<TemplateInput>
{
public:
	/// @brief The class object containing meta-information about this element type
	/// @todo change class name
	/// @todo assign a unique UUID
	/// @todo change display name
	using Class = ConcreteClass<"TemplateInput", "deadbeef-dead-beef-dead-beefdeadbeef"_uuid, "template driver input">;

	/// @brief This constructor attaches the input to its I/O component
	TemplateInput(std::reference_wrapper<TemplateIoComponent> ioComponent) :
		_ioComponent(ioComponent)
	{
	}

	/// @name Virtual Overrides for skill::DataPoint
	/// @{
	
	auto dataType() const -> const data::DataType & final;

	auto directions() const -> io::Directions final;

	auto forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool final;

	auto forEachEvent(const model::ForEachEventFunction &function) -> bool final;

	auto makeReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle> final;

	/// @}

	/// @name Virtual Overrides for AbstractInput
	/// @{

	auto ioComponent() const -> const TemplateIoComponent & final
	{
		return _ioComponent;
	}
	
	auto attachInput(memory::Array &dataArray, std::size_t &eventCount) -> void final;

	auto updateReadState(WriteSentinel &writeSentinel,
		std::chrono::system_clock::time_point timeStamp,
		const utils::eh::expected<std::reference_wrapper<const ReadCommand::Payload>, std::error_code> &payloadOrError,
		const CommonReadState::Changes &commonChanges,
		PendingEventList &eventsToRaise) -> void final;
		
	/// @}

	/// @brief A Xentara attribute containing the current value.
	/// @note This is a member of this class rather than of the attributes namespace, because the access flags
	/// and type may differ from class to class
	static const model::Attribute kValueAttribute;

private:
	/// @name Virtual Overrides for skill::DataPoint
	/// @{

	auto load(utils::json::decoder::Object &jsonObject, config::Context &context) -> void final;

	/// @}

	/// @brief The I/O component this input belongs to
	/// @todo give this a more descriptive name, e.g. "_device"
	std::reference_wrapper<TemplateIoComponent> _ioComponent;

	/// @brief The batch transaction this input belongs to, or nullptr if it hasn't been loaded yet.
	/// @todo give this a more descriptive name, e.g. "_poll"
	TemplateBatchTransaction *_batchTransaction { nullptr };

	/// @class xentara::plugins::templateDriver::TemplateInput
	/// @todo add information needed to decode the value from the payload of a read command, like e.g. a data offset.

	/// @brief The state
	/// @todo use the correct value type
	PerValueReadState<double> _state;
};

} // namespace xentara::plugins::templateDriver