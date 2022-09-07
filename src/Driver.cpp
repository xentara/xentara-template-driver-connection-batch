// Copyright (c) embedded ocean GmbH
#include "Driver.hpp"

#include <xentara/plugin/SharedFactory.hpp>
#include <xentara/io/Component.hpp>

namespace xentara::plugins::templateDriver
{

class Driver::Environment : public io::Driver::Environment
{
public:
	/// @class xentara::plugins::templateDriver::Driver::Environment
	/// @todo add constructor to perform global initialization, if necessary

	/// @class xentara::plugins::templateDriver::Driver::Environment
	/// @todo add destructor to perform global cleanup, if necessary

	/// @name Virtual Overrides for io::Driver::Environment
	/// @{

	auto createComponent(const io::ComponentClass &componentClass, plugin::SharedFactory<io::Component> &factory)
		-> std::shared_ptr<io::Component> final;

	/// @}
};

auto Driver::Environment::createComponent(const io::ComponentClass &componentClass,
	plugin::SharedFactory<io::Component> &factory)
	-> std::shared_ptr<io::Component>
{
	if (&componentClass == &TemplateIoComponent::Class::instance())
	{
		return factory.makeShared<TemplateIoComponent>();
	}

	/// @todo handle any additional top-level I/O component classes

	return nullptr;
}

auto Driver::createEnvironment() -> std::unique_ptr<io::Driver::Environment>
{
	return std::make_unique<Environment>();
}

} // namespace xentara::plugins::templateDriver