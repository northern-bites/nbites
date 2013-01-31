cmake_minimum_required(VERSION 2.8.3)

# Project name.
project(jointenactor)

# Find necessary libraries.
find_package(Boost REQUIRED)

# Include directories. 
#include_directories(${ALCOMMON_INCLUDE_DIR})

# Set source files here that need to be compiled. 
set(JOINT_ENACTOR_SRCS JointEnactorModule)

# Add project as a library. 
add_library(
  jointenactor
  STATIC
  ${JOINT_ENACTOR_SRCS}
)

# Link project to appropriate libraries. 
target_link_libraries(jointenactor 
  portals
  ${PROTOS_TARGET}
  ${ALCOMMON_LIBRARIES}
)