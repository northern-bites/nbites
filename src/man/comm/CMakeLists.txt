cmake_minimum_required( VERSION 2.8.3 )

# Set source files here that need to be compiled.
set( COMM_SRCS CommModule
               CommTimer
	       NetworkMonitor
               TeamConnect
               GameConnect
               UDPSocket
               )

# Add project as a library.
add_library(
  comm
  STATIC
  ${COMM_SRCS}
  )

# Link project to appropriate libraries.
target_link_libraries(
  comm
  support
  portals
  messages
  )
