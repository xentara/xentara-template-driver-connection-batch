# Connection Based, Batch Processing I/O Driver Template for Xentara

This repository contains skeleton code for a Xentara driver with the following features:

Connection based | Batch processing | Polymorphic Data Points
:--------------: | :--------------: | :---------------------:
YES              | YES              | NO

## Prerequisites

This driver template requires the Xentara development environment, as well as a Xentara licence. You can get a Xentara
licence in the [Xentara Online Shop](https://www.xentara.io/product/xentara-for-industrial-automation/).

The documentation for Xentara can be found at https://docs.xentara.io/xentara.

This driver template uses the Xentara Utility Library, as well as the Xentara Plugin Framework. The corresponding documentation can be found here:

https://docs.xentara.io/xentara-utils/  
https://docs.xentara.io/xentara-plugin/

## Build Environment

The repository include a [CMakeLists.txt](CMakeLists.txt) file for use with [CMake](https://cmake.org/), which allows you to build the source code
out of the box, as long as the Xentara development environment is installed. If you whish to use a different build system, you must generate the
necessary build configuration file yourself.

## Source Code Documentation

The source code in this repository is documented using [Doxygen](https://doxygen.nl/) comments. If you have Doxygen installed, you can
generate detailed documentation for all classes, functions etc., including a TODO list. To generate the documentation using CMake, just
build the target *docs* by executing the following command in your [build directory](https://cmake.org/cmake/help/latest/manual/cmake.1.html#generate-a-project-buildsystem):

~~~sh
cmake --build . --target docs
~~~

This will generate HTML documentation in the subdirectory *docs/html*.

## Xentara I/O Component Template

*(See [I/O Components](https://docs.xentara.io/xentara/xentara_io_components.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

[src/TemplateIoComponent.hpp](src/TemplateIoComponent.hpp)  
[src/TemplateIoComponent.cpp](src/TemplateIoComponent.cpp)

The I/O component template provides template code for devices that may be connected and disconnected while Xentara is running.
For such I/O components, Xentara will periodically try to reestablish communication to the physical device if it is disconnected.

The template code has the following features:

- The connection to the physical device is established during the [pre-operational stage](https://docs.xentara.io/xentara/xentara_operational_stages.html#xentara_operational_stages_pre_operational),
  and closed during the [post-operational stage](https://docs.xentara.io/xentara/xentara_operational_stages.html#xentara_operational_stages_post_operational).
- The [quality](https://docs.xentara.io/xentara/xentara_quality.html) of all skill data points belonging to the component
  is set to *Bad* if communication to the physical device breaks down.
- The I/O component tracks an error code for the communication with the physical device. If communication breaks down, this error code is pushed
  to the batch transaction and, from there, to the individual skill data points.
- The I/O component publishes a [Xentara task](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks) called *reconnect*,
  that checks the connection to the physical device, and attempts to reconnect if the communication has broken down.
- The I/O component publishes two [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) called *connected*
  and *disconnected*, that are fired when the connection to the physical device is establed or lost.

## Xentara I/O Batch Template

*(See [Batch Transactions](https://docs.xentara.io/xentara/xentara_batch_transactions.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

[src/TemplateBatchTransaction.hpp](src/TemplateBatchTransaction.hpp)  
[src/TemplateBatchTransaction.cpp](src/TemplateBatchTransaction.cpp)

The batch transaction template provides template code for a batch communications request that can read and write multiple skill data points at the same time.

The template code has the following features:

- All skill data points attached to a batch transaction share common [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code, which are maintained
  centrally in the batch transaction. These attributes are then inherited by the skill data points, so that they can be accessed as attributes of the skill data point as well.
- The batch transaction publishes a [Xentara task](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks) called *read*,
  which acquires the current values of all skill data points from the physical device using a read command.
- The batch transaction publishes a [Xentara event](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal changes
  to the [quality](https://docs.xentara.io/xentara/xentara_quality.html). This event
  is inherited by the skill data points, so that it can be accessed through the skill data points as well.
- The batch transaction publishes a [Xentara task](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_tasks) called *write*,
  that checks which outputs have pending output values, and writes those outputs to the physical device using a write command (if there are any).
- The batch transaction publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal if
  a write command was sent, or if a write error occurred. These events are *not* inherited by the skill data points, who have their own individual events instead.
  This is done so that the events of the individual outputs can be fired individually for only those outputs that were actually written.
- If a communication breakdown is detected during a read command, the I/O component is notified, and all skill data points in this or all other batch transactions
  are invalidated.
- No communication with the physical device is attempted if the connection is not up.

## Xentara Skill Data Point Templates

*(See [Skill Data Points](https://docs.xentara.io/xentara/xentara_skill_data_points.html) in the [Xentara documentation](https://docs.xentara.io/xentara/))*

### Input Template

[src/TemplateInput.hpp](src/TemplateInput.hpp)  
[src/TemplateInput.cpp](src/TemplateInput.cpp)  

The input template provides template code for a read-only skill data point whose value is read using a batch transaction.

The template code has the following features:

- The input inherits [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code from the
  batch transaction, and shares them with all other skill data points belonging to the same batch transaction.
- The input publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal changes
  to the value and [quality](https://docs.xentara.io/xentara/xentara_quality.html). The event to signal
  a change in quality is inherited from the batch transaction, and shared with all other skill data points belonging to the same batch transaction.

### Output Template

[src/TemplateOutput.hpp](src/TemplateOutput.hpp)  
[src/TemplateOutput.cpp](src/TemplateOutput.cpp)

The output template provides template code for a read/write skill data point whose value must be read and written using individual commands for each output.

The template code has the following features:

- The input and output values are handled entirely separately. A written output value is not reflected in the input value until
  it has been read back from the I/O component by the batch transaction. This is necessary because the I/O component might reject or
  modify the written value.
- The value of the output is not sent to the I/O component directly when it is written, but placed in a queue to be written by the batch transaction.
- The output inherits [Xentara attributes](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_attributes)
  for update time, [quality](https://docs.xentara.io/xentara/xentara_quality.html) and error code from the
  batch transaction, and shares them with all other skill data points belonging to the same batch transaction.
- The output publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal changes
  to the value and [quality](https://docs.xentara.io/xentara/xentara_quality.html). The event to signal
  a change in quality is inherited from the batch transaction, and shared with all other skill data points belonging to the same batch transaction.
- The output publishes [Xentara events](https://docs.xentara.io/xentara/xentara_element_members.html#xentara_events) to signal if
  a new value was written, or if a write error occurred.
