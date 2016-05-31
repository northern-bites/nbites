cmake_minimum_required( VERSION 2.8.0 )
project( UNSWALK )

add_definitions(-DLINUX -DRELEASE -DTARGET_ROBOT)

set( unswalk_srcs
  utils/basic_maths.hpp
  utils/body.hpp
  utils/Logger.hpp
  gamecontroller/RoboCupGameControlData.cpp
  gamecontroller/GameController.cpp
  blackboard/Blackboard.cpp
  perception/kinematics/UNSWKinematics.cpp
  types/JointValues.hpp
  types/UNSWSensorValues.hpp
  types/Odometry.hpp
  types/XYZ_Coord.hpp
  touch/FilteredTouch.cpp
  touch/FeetState.cpp
  touch/Touch.hpp
  touch/TorsoStateFilter.cpp
  utils/angles.hpp
  UNSWalkProvider.cpp
  IKinematics.cpp
  generator/Generator.hpp
  generator/BodyModel.cpp
  generator/Walk2014Generator.cpp
)



add_library(
	unswalk
	STATIC
	${unswalk_srcs} )