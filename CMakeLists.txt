cmake_minimum_required(VERSION 3.22)

# TODO: rename project and add description, URL etc.
project(xentara-template-driver	LANGUAGES CXX)

# Force the use of C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Tell MSVC to set __cplusplus to the correct value
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	add_compile_options("/Zc:__cplusplus")
endif()

# Find the Xentara utility and plugin libraries
find_package(XentaraUtils REQUIRED)
find_package(XentaraPlugin REQUIRED)

# Add the plugin library target
add_library(
	${PROJECT_NAME} MODULE

	"src/AbstractInput.hpp"
	"src/AbstractOutput.hpp"
	"src/Attributes.cpp"
	"src/Attributes.hpp"
	"src/CommonReadState.cpp"
	"src/CommonReadState.hpp"
	"src/CustomError.cpp"
	"src/CustomError.hpp"
	"src/Driver.cpp"
	"src/Driver.hpp"
	"src/PerValueReadState.cpp"
	"src/PerValueReadState.hpp"
	"src/Plugin.cpp"
	"src/Plugin.hpp"
	"src/ReadCommand.hpp"
	"src/ReadTask.hpp"
	"src/SingleValueQueue.hpp"
	"src/TemplateInput.cpp"
	"src/TemplateInput.hpp"
	"src/TemplateIoBatch.cpp"
	"src/TemplateIoBatch.hpp"
	"src/TemplateIoComponent.cpp"
	"src/TemplateIoComponent.hpp"
	"src/TemplateOutput.cpp"
	"src/TemplateOutput.hpp"
	"src/Types.hpp"
	"src/WriteCommand.hpp"
	"src/WriteState.cpp"
	"src/WriteState.hpp"
	"src/WriteTask.hpp"
)

# Link against the Xentara utility and plugin libraries
target_link_libraries(
	${PROJECT_NAME}

	PRIVATE
		Xentara::xentara-utils
		Xentara::xentara-plugin
)

# Make output names adhere to Xentara convetions under Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(
		${PROJECT_NAME}
		
		PROPERTIES
# TODO: adjust output name to match project name
			OUTPUT_NAME XentaraTemplateDriver
			DEBUG_POSTFIX d
	)
endif()

# Try to find doxygen
find_package(Doxygen QUIET)

# Generate Doxygen documentation, if doxygen was found
if(Doxygen_FOUND)
	# Get the list of source files from the library
	get_target_property(target_sources ${PROJECT_NAME} SOURCES)

	# Set some useful doxygen options
	set(DOXYGEN_QUIET YES)
	set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs")
	set(DOXYGEN_EXTRACT_PRIVATE YES)
	set(DOXYGEN_GENERATE_TREEVIEW YES)
	set(DOXYGEN_EXTRACT_ANON_NSPACES YES)
	set(DOXYGEN_SUBGROUPING NO)
	set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)

	# Under Windows, Doxygen generates file names that are too long for it to handle, resulting in
	# errors when generating the documentation. As a workaround, we tell Doxygen to generate short file names.
	if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
		set(DOXYGEN_SHORT_NAMES YES)
	endif()

	# Get the location of the tag files for the Xentara documentation
	get_target_property(xentara_utils_tag_file Xentara::xentara-utils XENTARA_DOXYGEN_TAG_FILE)
	get_target_property(xentara_plugin_tag_file Xentara::xentara-plugin XENTARA_DOXYGEN_TAG_FILE)
	set(DOXYGEN_TAGFILES
		"${xentara_utils_tag_file}=https://docs.xentara.io/xentara-utils"
		"${xentara_plugin_tag_file}=https://docs.xentara.io/xentara-plugin"
	)

	# Add a target to generate the doxygen documentation
	doxygen_add_docs(docs ${target_sources})
endif()

install_xentara_plugin(${PROJECT_NAME})