
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
                 ChannelImageViewer.cpp
		 BallEKFViewer.cpp
		 FieldViewer.cpp
		 EKFDataViewer.cpp
<<<<<<< HEAD
		 #GraphViewer.cpp
		 #R/RTool.cpp
=======
		 VisionViewer.cpp
		 CollapsibleImageViewer.cpp
>>>>>>> master
		 )

set( viewer_headers MemoryViewer.h
                    MObjectViewer.h
                    BMPImageViewer.h
                    ChannelImageViewer.h
		    BallEKFViewer.h
		    FieldViewer.h
		    EKFDataViewer.h
<<<<<<< HEAD
		    #GraphViewer.h
		    #R/RTool.h
=======
		    VisionViewer.h
		    CollapsibleImageViewer.h
>>>>>>> master
		    )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(viewer_headers_moc ${viewer_headers})
Qt4_wrap_ui(viewer_forms_headers ${viewer_forms})

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
  vision
  _sensors
  _robot_connect
  ${QT_LIBRARIES}
  #${R_LIBRARY} 
  #${QWT_LIBRARY}

  )