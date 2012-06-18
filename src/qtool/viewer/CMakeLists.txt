
############ viewer

project( viewer )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS}) #-DR_HOME=/usr/lib/R)

#find_library(R_LIBRARY R PATHS)
#find_library(QWT_LIBRARY qwt PATHS)

########### Sources

set( viewer_srcs MemoryViewer.cpp
                 MObjectViewer.cpp
                 BMPImageViewer.cpp
                 BMPImageViewerListener.cpp
                 ChannelImageViewer.cpp
                 BallEKFViewer.cpp
                 FieldViewer.cpp
                 EKFDataViewer.cpp
                 CollapsibleImageViewer.cpp
		 		 BotLocs.cpp
		 		 RobotField.cpp
		 		 ParticleViewer.cpp
                 )

set( viewer_headers MemoryViewer.h
                    MObjectViewer.h
                    BMPImageViewer.h
                    BMPImageViewerListener.h
                    ChannelImageViewer.h
                    BallEKFViewer.h
                    FieldViewer.h
                    EKFDataViewer.h
                    CollapsibleImageViewer.h
		    		BotLocs.h
		    		RobotField.h
		    		ParticleViewer.h
                    )


########### Separate VisionViewer Sources

set( vision_vw_srcs VisionViewer.cpp
                    FilteredThresholdedViewer.cpp
		    )

set( vision_vw_headers VisionViewer.h
                       FilteredThresholdedViewer.h
		       )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(viewer_headers_moc ${viewer_headers})
Qt4_wrap_ui(viewer_forms_headers ${viewer_forms})

Qt4_wrap_cpp(vision_vw_headers_moc ${vision_vw_headers})
Qt4_wrap_ui(vision_vw_forms_headers ${vision_vw_forms})

add_library(viewer
    STATIC
    ${viewer_srcs}
    ${viewer_headers_moc}
    ${viewer_forms_headers} )

target_link_libraries(
  viewer
  treemodel
  image
  memory
  ${QT_LIBRARIES}
  )

############# Separate VisionViewer library
add_library(vision_viewer
    STATIC
    ${vision_vw_srcs}
    ${vision_vw_headers_moc}
    ${vision_vw_forms_headers} )

target_link_libraries(
  vision_viewer
  viewer
  vision
  _sensors
  _robot_connect
  ${QT_LIBRARIES}
  #${R_LIBRARY}
  #${QWT_LIBRARY}

  )
