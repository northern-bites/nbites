cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(MOTION_SRCS MotionModule
                NullProvider)

include_directories( bhwalk )

# Add project as a library.
add_library(
  motion
  STATIC
  ${MOTION_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(motion
  portals
  messages
  common
  bhwalk
)

add_subdirectory( bhwalk )