cmake_minimum_required(VERSION 2.8.3)

# Project name.
project(localization)

# Include directories.
#include_directories( ... )

# Set source files here that need to be compiled.
set(LOCALIZATION_SRCS LocalizationModule
                      ParticleFilter
		      Particle
		      MotionModel
		      SensorModel
#		      FakeLocInputModule
		      VisionSystem
		      MotionSystem
		      LineSystem
)

# Add project as a library.
add_library(
  localization
  STATIC
  ${LOCALIZATION_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(localization
  support
  portals
  messages)

if ( OFFLINE )
  nbites_add_test(LocalizationTest LocalizationTest.cpp localization)
endif ( OFFLINE )
