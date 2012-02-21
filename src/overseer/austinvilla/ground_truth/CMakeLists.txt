cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_FLAGS "-Wno-deprecated")

rosbuild_init()

#for nbites overseer
include_directories(../../../man/include)
include_directories(../../../qtool/overseer)
include_directories(../../../)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
#rosbuild_gensrv()

#common commands for building c++ executables and libraries

rosbuild_add_library(field_provider src/lib/field_provider.cpp)

rosbuild_add_executable(detect src/nodes/detect.cc)
target_link_libraries(detect field_provider overseer_server)

rosbuild_add_executable(calibrate src/nodes/calibrate.cc)
target_link_libraries(calibrate field_provider)

