cmake_minimum_required( VERSION 2.8.3 )

############################ PROJECT NAME
# The name of this cmake project
project( guardian )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( GUARDIAN_SRCS GuardianModule
  ClickableButton
  SoundPaths
  )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

add_library(
  guardian
  STATIC
  ${GUARDIAN_SRCS}
  )

############################ SET LIBRARIES TO LINK WITH
target_link_libraries(
  guardian
  common
  portals
  messages
  )
