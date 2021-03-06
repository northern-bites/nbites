# required libraries for python

find_package( PythonLibs   REQUIRED )
include_directories( ${PYTHON_INCLUDE_DIR} )

# for configuring the player switch
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/Switch.py.in"
  "${PROJECT_BINARY_DIR}/behaviors/players/Switch.py"
  )

# CPP sources
set( BEHAVIORS_SRCS BehaviorsModule
  PyObjects
  CombinationObjects
  PyConstants
  PyInterface
  )

# Python sources
set( BEHAVIORS_PYTHON_SRCS __init__.py
                        Brain.py
                        GameController.py
                        FallController.py
                        Stability.py
                        Leds.py
                        StiffnessModes.py
                        SweetMoves.py
                        Say.py
                        )

# The switch file
set ( BEHAVIORS_SWITCH ${CMAKE_CURRENT_BINARY_DIR}/players/Switch.py)

# Python subdirectories
set( BEHAVIORS_PYTHON_SUBDIRS headTracker
                           kickDecider
                           navigator
                           players
                           robots
                           typeDefs
                           util )

# The CPP library
add_library(
  behaviors
  STATIC
  ${BEHAVIORS_SRCS}
  )

# How to install the python files for rsync
set( PYTHON_INSTALL_DIR
  lib/python )

install(
 FILES ${BEHAVIORS_PYTHON_SRCS}
 DESTINATION ${PYTHON_INSTALL_DIR}
 CONFIGURATIONS "" Debug Release
 )
install(
 FILES ${BEHAVIORS_SWITCH}
 DESTINATION ${PYTHON_INSTALL_DIR}/players/
 CONFIGURATIONS "" Debug Release
 )
install(
 DIRECTORY ${BEHAVIORS_PYTHON_SUBDIRS}
 DESTINATION ${PYTHON_INSTALL_DIR}/
 CONFIGURATIONS "" Debug Release
 PATTERN "*.py"
 )

target_link_libraries(
  behaviors
  support
  portals
  messages
  ${PYTHON_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
  )
