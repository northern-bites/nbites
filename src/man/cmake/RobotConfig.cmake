# Build options

cmake_minimum_required( VERSION 2.8.3 )

set( @ROBOT_PLAYER_NUMBER@
  "2"
  CACHE STRING
  "The player number for the robot."
)

set( @ROBOT_TEAM_NUMBER@
  "16"
  CACHE STRING
  "The team number for the robot."
)

set( @REMOTE_ADDRESS@
  "river.local"
  CACHE STRING
  "The address to which the code will be installed."
)

set ( @PYTHON_PLAYER@
  pNone
  CACHE STRING
  "Choose the player to be imported in Switch.py"
  )

# So that files can be configured
set( REMOTE_ADDRESS ${@REMOTE_ADDRESS@})
set( PLAYER_NUMBER ${@ROBOT_PLAYER_NUMBER@})
set( TEAM_NUMBER ${@ROBOT_TEAM_NUMBER@})
set( PYTHON_PLAYER ${@PYTHON_PLAYER@})

set( INSTALL_LOCATION
  "nbites/"
  CACHE STRING
  "The folder on the robot where the code will end up."
)