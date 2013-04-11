cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(MOTION_SRCS MotionModule
                NullProvider
		BaseFreezeCommand
		BodyJointCommand
		ChoppedCommand
		SmoothChoppedCommand
		LinearChoppedCommand
		ChopShop
		ScriptedProvider
		HeadJointCommand
		HeadProvider)

include_directories( bhwalk )

add_subdirectory( bhwalk )

# Add project as a library.
add_library(
  motion
  STATIC
  ${MOTION_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(motion
  portals
  messages
  common
  bhwalk
)