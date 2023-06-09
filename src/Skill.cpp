// Copyright (c) embedded ocean GmbH
#include "Skill.hpp"

#include <xentara/skill/Element.hpp>
#include <xentara/skill/ElementFactory.hpp>

namespace xentara::plugins::templateDriver
{

auto Skill::createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
	-> std::shared_ptr<skill::Element>
{
	if (&elementClass == &TemplateIoComponent::Class::instance())
	{
		return factory.makeShared<TemplateIoComponent>();
	}

	/// @todo handle any additional top-level I/O component classes

	return nullptr;
}

} // namespace xentara::plugins::templateDriver