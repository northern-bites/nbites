cmake_minimum_required(VERSION 2.8.3)

# Project name.
project(sensors)

# Find necessary libraries.
find_package(Boost REQUIRED)

# Include directories. 
#include_directories(${ALCOMMON_INCLUDE_DIR})

# Set source files here that need to be compiled. 
set(SENSORS_SRCS SensorsModule)

# Add project as a library. 
add_library(
  sensors
  STATIC
  ${SENSORS_SRCS}
)

# Link project to appropriate libraries. 
target_link_libraries(sensors 
  portals
  messages
  ${ALCOMMON_LIBRARIES}
)