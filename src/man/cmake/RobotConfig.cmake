# Build options

cmake_minimum_required( VERSION 2.8.3 )

set( @PLAYER_NUMBER@ )
  "2"
  CACHE STRING
  "The player number for the robot."
)

set( @TEAM_NUMBER@ )
  "16"
  CACHE STRING
  "The team number for the robot."
)

set( @REMOTE_ADDRESS@
  "river.local"
  CACHE STRING
  "The address to which the code will be installed."
)
# So that files can be configured
set( REMOTE_ADDRESS ${@REMOTE_ADDRESS@})
set( PLAYER_NUMBER ${@PLAYER_NUMBER@})
set( TEAM_NUMBER ${@TEAM_NUMBER@})

set( INSTALL_LOCATION
  "nbites/"
  CACHE STRING
  "The folder on the robot where the code will end up."
)