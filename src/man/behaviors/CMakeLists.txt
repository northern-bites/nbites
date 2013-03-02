
############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
find_package( PythonLibs   REQUIRED )
find_package( Boost COMPONENTS python REQUIRED )

############################ CONFIG.H GENERATION
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/Switch.py.in"
  "${PROJECT_BINARY_DIR}/behaviors/players/Switch.py"
  )

############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${PYTHON_INCLUDE_DIR}
                     ${CFG_DIR}
		     ${BOOST_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

set( BEHAVIORS_SRCS BehaviorsModule
		 PyConstants
                 )


set( BEHAVIORS_PYTHON_SRCS __init__.py
                        Brain.py
                        GameController.py
                        GameStates.py
                        FallController.py
			Stability.py
                        FallStates.py
                        Leds.py
                        )

set ( BEHAVIORS_SWITCH ${CMAKE_CURRENT_BINARY_DIR}/players/Switch.py)

set( BEHAVIORS_PYTHON_SUBDIRS headTracking
     			   kickDecider
                           navigator
                           playbook
                           players
                           robots
                           typeDefs
                           util )


############################ BUILD TARGETS

add_library(
  behaviors
  STATIC
  ${BEHAVIORS_SRCS}
  )
#install(
#  TARGETS ${BEHAVIORS_TARGET}
#  ARCHIVE DESTINATION ${OUTPUT_ROOT_DIR_LIB}
#  CONFIGURATIONS Debug
#  )

#install(
#  files ${BEHAVIORS_PYTHON_SRCS}
#  DESTINATION ${BEHAVIORS_PYTHON_TARGET_DIR}
#  CONFIGURATIONS "" Debug Release
#  )
#install(
#  FILES ${BEHAVIORS_SWITCH}
#  DESTINATION ${BEHAVIORS_PYTHON_TARGET_DIR}/players/
#  CONFIGURATIONS "" Debug Release
#  )
#install(
#  DIRECTORY ${BEHAVIORS_PYTHON_SUBDIRS}
#  DESTINATION ${BEHAVIORS_PYTHON_TARGET_DIR}/
#  CONFIGURATIONS "" Debug Release
#  PATTERN "*.py"
#  )


############################ LINKER LANGUAGE

#set_target_properties(
#    ${BEHAVIORS_TARGET}
#    PROPERTIES LINKER_LANGUAGE CXX
#    )


############################ SET LIBRARIES TO LINK WITH
target_link_libraries(
  behaviors
  ${PYTHON_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
  common
  portals
  messages
  )

