cmake_minimum_required( VERSION 2.8.0 )
project( UNSWALK )

add_definitions(-DLINUX -DRELEASE -DTARGET_ROBOT)

set( unswalk_srcs
  UNSWalkProvider.cpp
  IKinematics.cpp
  generator/Walk2014Generator.cpp
  generator/BodyModel.cpp  
)



add_library(
	unswalk
	STATIC
	${unswalk_srcs} )