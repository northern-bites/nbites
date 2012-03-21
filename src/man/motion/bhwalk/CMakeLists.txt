
project( BHWALK )
cmake_minimum_required( VERSION 2.8.0 )

# Include  the basic definitions accross all projects
include( ../../cmake/base_definitions.cmake )

include_directories( . )

add_definitions(-DLINUX -DRELEASE)

set( bhwalk_srcs BHWalkProvider.cpp
                 WalkingEngine.cpp
                 MotionRequest.cpp
                 TorsoMatrixProvider.cpp
                 Robot/RobotModel.cpp
                 Platform/BHAssert.cpp
                 Platform/File.cpp
                 Platform/SystemCall.cpp
                 Tools/Enum.cpp
                 Tools/Global.cpp
                 Tools/Math/Matrix.cpp
                 Tools/Math/Pose2D.cpp
                 Tools/Math/RotationMatrix.cpp
                 Tools/Streams/InOut.cpp
                 Tools/Streams/InStreams.cpp
                 Tools/Streams/OutStreams.cpp
                 Tools/Streams/StreamHandler.cpp
                 Tools/Streams/Streamable.cpp
                 Tools/Configuration/ConfigMap.cpp
                 Tools/Configuration/ConfigMapParser.cpp )

add_library(
	bhwalk
	STATIC
	${bhwalk_srcs} )