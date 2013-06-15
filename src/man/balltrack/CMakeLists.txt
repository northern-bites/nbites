cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(BALLTRACK_SRCS BallTrackModule
#                   NaiveBallFilter
#		   FakeBallTrackInput
		   KalmanFilter
		   MMKalmanFilter
)

# Add project as a library.
add_library(
  balltrack
  STATIC
  ${BALLTRACK_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(balltrack
                      support
		      portals
		      messages
		      )

# GTEST

if (OFFLINE)
  nbites_add_test(KalmanFilterTest KalmanFilterTest.cpp
    balltrack
    )
endif()
