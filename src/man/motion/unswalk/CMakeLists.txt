cmake_minimum_required( VERSION 2.8.0 )
project( UNSWALK )

add_definitions(-DLINUX -DRELEASE -DTARGET_ROBOT)

set(CMAKE_CXX_FLAGS "-lboost_program_options")

set( unswalk_srcs
  utils/basic_maths.hpp
  utils/body.hpp
  utils/Timer.hpp
  utils/Logger.hpp
  gamecontroller/RoboCupGameControlData.cpp
  gamecontroller/GameController.cpp
  blackboard/Blackboard.cpp
  perception/kinematics/Pose.cpp
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
  effector/Effector.hpp
  generator/Generator.hpp
  generator/BodyModel.cpp
  generator/WalkCycle.cpp
  generator/PendulumModel.cpp
  generator/StandGenerator.cpp
  generator/ClippedGenerator.cpp
  generator/ActionGenerator.cpp
  generator/DistributedGenerator.cpp
  generator/WalkEnginePreProcessor.cpp
  generator/Walk2014Generator.cpp
)






add_library(
	unswalk
	STATIC
	${unswalk_srcs} )