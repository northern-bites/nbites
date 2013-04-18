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

# EJ-Make this better
# if offline on, then we're prob running unit tests
# IF ( OFFLINE )
#   add_executable(
#     balltrack_test
#     tests/KalmanFilterTest.cpp
#     )

#   target_link_libraries(
#     balltrack_test
#     balltrack
#     pthread
#     ${GTEST_LIBRARY}
#     )

#   add_test(
#     balltrack_test
#     balltrack_test
#     )

#   add_custom_target(check
#                     DEPENDS balltrack_test)
# ENDIF ( OFFLINE )
