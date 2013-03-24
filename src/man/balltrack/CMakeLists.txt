cmake_minimum_required(VERSION 2.8.3)

# Project name.
project(balltrack)

# Include directories.
#include_directories( ... )

# Set source files here that need to be compiled.
set(BALLTRACK_SRCS BallTrackModule
                   NaiveBallFilter
		   FakeBallTrackInput
)

# Add project as a library.
add_library(
  balltrack
  STATIC
  ${BALLTRACK_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(balltrack
                      common
		      portals
		      messages
		      ${PROTOS_TARGET})
