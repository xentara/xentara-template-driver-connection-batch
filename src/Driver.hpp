// Copyright (c) embedded ocean GmbH
#pragma once

#include "TemplateIoComponent.hpp"
#include "TemplateIoBatch.hpp"
#include "TemplateOutput.hpp"
#include "TemplateInput.hpp"

#include <xentara/utils/core/Uuid.hpp>
#include <xentara/io/Driver.hpp>

#include <string_view>

namespace xentara::plugins::templateDriver
{

using namespace std::literals;

/// @brief The driver class
///
/// This class registers all the elements the driver provides, and creates the driver runtime environment.
class Driver final : public io::Driver
{
public:
	/// @name Virtual Overrides for io::Driver
	/// @{

	auto name() const -> std::u16string_view final
	{
		/// @todo change class name
		return u"TemplateDriver"sv;
	}

	auto uuid() const -> utils::core::Uuid final
	{
		/// @todo assign a unique UUID
		return "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"_uuid;
	}

	auto registerObjects(Registry &registry) -> void final
	{
		/// @todo register any additional element classes
		registry <<
			TemplateIoComponent::Class::instance() <<
			TemplateIoBatch::Class::instance() <<
			TemplateOutput::Class::instance() <<
			TemplateInput::Class::instance();
	}

	auto createEnvironment() -> std::unique_ptr<io::Driver::Environment> final;
	
	/// @}

private:
	/// @class xentara::plugins::templateDriver::Driver::Environment
	/// @brief The driver runtime environment
	class Environment;
};

} // namespace xentara::plugins::templateDriver