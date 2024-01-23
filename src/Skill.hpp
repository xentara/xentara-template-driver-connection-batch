// Copyright (c) embedded ocean GmbH
#pragma once

#include "TemplateIoComponent.hpp"
#include "TemplateIoTransaction.hpp"
#include "TemplateOutput.hpp"
#include "TemplateInput.hpp"

#include <xentara/skill/Skill.hpp>
#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

/// @brief The skill
class Skill final : public skill::Skill
{
public:
	/// @class xentara::plugins::templateDriver::Skill
	/// @todo add constructor to perform global initialization, if necessary

	/// @class xentara::plugins::templateDriver::Skill
	/// @todo add destructor to perform global cleanup, if necessary

	/// @name Virtual Overrides for skill::Skill
	/// @{

	auto createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
		-> std::shared_ptr<skill::Element> final;

	/// @}

private:
	/// @brief The skill class
	/// @todo change class name
	/// @todo assign a unique UUID
	using Class = ConcreteClass<Skill,
		"TemplateDriver",
		"deadbeef-dead-beef-dead-beefdeadbeef"_uuid,
		TemplateIoComponent::Class,
		TemplateIoTransaction::Class,
		TemplateOutput::Class,
		TemplateInput::Class>;

	/// @brief The skill class object
	static Class _class;
};

} // namespace xentara::plugins::templateDriver