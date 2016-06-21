cmake_minimum_required( VERSION 2.8.0 )
project( BHWALK )

add_definitions(-DLINUX -DRELEASE -DTARGET_ROBOT)

set( bhwalk_srcs 
  BHWalkProvider.cpp
  MotionSelector.cpp
  WalkingEngine.cpp
  WalkingEngineKicks.cpp
  WalkingEngineTools.cpp
  KickEngine/KickEngine.cpp
  KickEngine/KickEngineData.cpp
  KickEngine/KickEngineParameters.cpp
  Modules/Infrastructure/NaoProvider.cpp
  Modules/Sensing/TorsoMatrixProvider.cpp
  Modules/Sensing/RobotModelProvider.cpp
  Modules/Sensing/FallDownStateDetector.cpp
  Modules/Sensing/InertiaSensorCalibrator.cpp
  Modules/Sensing/InertiaSensorFilter.cpp
  Modules/Sensing/JointFilter.cpp
  Modules/Sensing/SensorFilter.cpp
  Modules/Sensing/GroundContactDetector.cpp
  Modules/Infrastructure/NaoProvider.cpp
  Representations/Infrastructure/Image.cpp
  Representations/Infrastructure/Thumbnail.cpp
  Representations/Infrastructure/RobotInfo.cpp
  Representations/Infrastructure/TeamInfo.cpp
  Representations/Infrastructure/GameInfo.cpp
  Representations/Infrastructure/CameraInfo.cpp
  Representations/Configuration/CameraSettings.cpp
  Representations/Configuration/FieldDimensions.cpp
  Representations/Sensing/RobotModel.cpp
  Representations/Perception/ImageCoordinateSystem.cpp
  Representations/Perception/ColorReference.cpp
  Representations/Blackboard.cpp
  Representations/MotionControl/KickRequest.cpp
  Platform/File.cpp
  Platform/BHAssert.cpp
  Platform/Linux/NaoBody.cpp
  Platform/Linux/SystemCall.cpp
  Platform/Linux/SoundPlayer.cpp
  Platform/Linux/Semaphore.cpp
  Tools/Enum.cpp
  Tools/Global.cpp
  Tools/Math/Matrix.cpp
  Tools/Math/Pose2D.cpp
  Tools/Math/RotationMatrix.cpp
  Tools/Math/Geometry.cpp
  Tools/Streams/InOut.cpp
  Tools/Streams/InStreams.cpp
  Tools/Streams/OutStreams.cpp
  Tools/Streams/StreamHandler.cpp
  Tools/Streams/Streamable.cpp
  Tools/Streams/SimpleMap.cpp
  Tools/Configuration/ConfigMap.cpp
  Tools/Configuration/ConfigMapParser.cpp
  Tools/Optimization/ParticleSwarm.cpp
  Tools/Module/Module.cpp
  Tools/Debugging/TimingManager.cpp
  Tools/Debugging/DebugRequest.cpp
  Tools/MessageQueue/MessageQueueBase.cpp
  Tools/MessageQueue/MessageQueue.cpp
  Tools/MessageQueue/OutMessage.cpp
  Tools/MessageQueue/InMessage.cpp
  Util/snappy/lib-source/snappy.cc
  Util/snappy/lib-source/snappy-c.cc
  Util/snappy/lib-source/snappy-sinksource.cc
  Util/snappy/lib-source/snappy-stubs-internal.cc
)

# Install configs
if( NOT OFFLINE )
    set( BHWALK_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Config )
    install( DIRECTORY ${BHWALK_CONFIG_DIR}
    DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif()

add_library(
	bhwalk
	STATIC
	${bhwalk_srcs} )
