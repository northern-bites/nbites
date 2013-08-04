cmake_minimum_required( VERSION 2.8.3 )

# Set source files here that need to be compiled.
set( GUARDIAN_SRCS GuardianModule
  ClickableButton
  SoundPaths
  )

# Add project as a library.
add_library(
  guardian
  STATIC
  ${GUARDIAN_SRCS}
  )

# Link project to appropriate libraries.
target_link_libraries(
  guardian
  support
  portals
  messages
  )
