
project( BHWALK )
cmake_minimum_required( VERSION 2.8.0 )

# Include  the basic definitions accross all projects
include( ../../cmake/base_definitions.cmake )

add_definitions(-DLINUX -DRELEASE -DTARGET_ROBOT)

set( bhwalk_srcs BHWalkProvider.cpp
                 WalkingEngine.cpp
                 WalkingEngineKick.cpp
                 Modules/Infrastructure/NaoProvider.cpp
                 Modules/Sensing/TorsoMatrixProvider.cpp
                 Modules/Sensing/RobotModelProvider.cpp
                 Modules/Sensing/FallDownStateDetector.cpp
                 Modules/Sensing/InertiaSensorCalibrator.cpp
                 Modules/Sensing/InertiaSensorFilter.cpp
                 Modules/Sensing/InertiaSensorInspector.cpp
                 Modules/Sensing/JointFilter.cpp
                 Modules/Sensing/SensorFilter.cpp
                 Representations/Sensing/RobotModel.cpp
                 Representations/MotionControl/MotionSelection.cpp
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