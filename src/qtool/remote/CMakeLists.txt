
############ QTool/Remote

project( remote )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( remote_srcs
    RobotFinder
    RobotSelect
    RemoteDataFinder
    )

set( remote_headers 
	RobotFinder.h
	RobotSelect.h
	RemoteDataFinder.h )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

############ Qt4 goodness

Qt4_wrap_cpp(remote_headers_moc ${remote_headers})

######### The Library

add_library(remote
    STATIC
    ${remote_srcs}
    ${remote_headers_moc}
    )


target_link_libraries(
  remote
  data
  ${QT_LIBRARIES}
  ${QT_QTNETWORK_LIBRARY}
  )