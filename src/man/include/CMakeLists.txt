############################ PROJECT NAME
# The name of this cmake project
PROJECT( NBINCLUDE )
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )

############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )

############################ External packages

FIND_PACKAGE( Threads )

############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS(
  ${CMAKE_THREAD_LIBS_INIT}
  )

###### NBINCLUDE TARGET ####

SET( NBINCLUDE_SRCS NBMatrixMath
  NBMath
  dsp
  SensorMonitor
  BulkMonitor
  synchro/mutex
  synchro/synchro
  synchro/messaging
  CoordFrame4D
  CoordFrame3D
  Profiler
  )

ADD_LIBRARY(
  ${NBINCLUDE_TARGET}
  STATIC
  ${NBINCLUDE_SRCS}
  )