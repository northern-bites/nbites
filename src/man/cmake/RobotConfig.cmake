# Build options

cmake_minimum_required( VERSION 2.8.3 )

set( @ROBOT_NAME@
  "river"
  CACHE STRING
  "The name of the robot."
)

set( @REMOTE_ADDRESS@
  "river.local"
  CACHE STRING
  "The address to which the code will be installed."
)
# So that upload.sh can be configured
set( REMOTE_ADDRESS ${@REMOTE_ADDRESS@})

set( INSTALL_LOCATION
  "nbites/"
  CACHE STRING
  "The folder on the robot where the code will end up."
)