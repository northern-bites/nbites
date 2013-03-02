cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(TESTMODULE_SRCS TestModule)

# Add project as a library.
add_library(
  testmod
  STATIC
  ${TESTMODULE_SRCS}
)

# Link project ot appropriate libraries.
target_link_libraries(testmod
  portals
  messages
  ${ALCOMMON_LIBRARIES}
)