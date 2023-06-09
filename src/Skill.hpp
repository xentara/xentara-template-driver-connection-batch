// Copyright (c) embedded ocean GmbH
#pragma once

#include "TemplateIoComponent.hpp"
#include "TemplateBatchTransaction.hpp"
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
	/// @brief The class object containing meta-information about this skill
	class Class final : public skill::Skill::Class
	{
	public:
		/// @name Virtual Overrides for skill::Skill::Class
		/// @{

		auto name() const -> std::string_view final
		{
			/// @todo change class name
			return "TemplateDriver"sv;
		}

		auto uuid() const -> utils::core::Uuid final
		{
			/// @todo assign a unique UUID
			return "deadbeef-dead-beef-dead-beefdeadbeef"_uuid;
		}

		auto registerElements(Registry &registry) -> void final
		{
			/// @todo register any additional element classes
			registry <<
				TemplateIoComponent::Class::instance() <<
				TemplateBatchTransaction::Class::instance() <<
				TemplateOutput::Class::instance() <<
				TemplateInput::Class::instance();
		}

		auto createSkill() -> std::unique_ptr<skill::Skill> final
		{
			return std::make_unique<Skill>();
		}
	
		/// @}
	};

	/// @class xentara::plugins::templateDriver::Skill
	/// @todo add constructor to perform global initialization, if necessary

	/// @class xentara::plugins::templateDriver::Skill
	/// @todo add destructor to perform global cleanup, if necessary

	/// @name Virtual Overrides for skill::Skill
	/// @{

	auto createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
		-> std::shared_ptr<skill::Element> final;

	/// @}
};

} // namespace xentara::plugins::templateDriver