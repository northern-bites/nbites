cmake_minimum_required( VERSION 2.8.3 )

############################ PROJECT NAME
# The name of this cmake project
project( comm )

############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
find_package( PythonLibs   REQUIRED )
#FIND_PACKAGE( Threads  REQUIRED )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( COMM_SRCS Comm
               CommTimer
	       NetworkMonitor
               TeamConnect
               TeamMember
               GameConnect
               GameData
               UDPSocket
               RoboCupGameControlData
#               PyComm
               )

# set( COMM_PYTHON_SRCS )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

add_library(
  comm
  STATIC
  ${COMM_SRCS}
  )

############################ SET LIBRARIES TO LINK WITH
target_link_libraries(
  comm
  portals
  messages
  )
