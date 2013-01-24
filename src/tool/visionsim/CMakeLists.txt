cmake_minimum_required( VERSION 2.8.3 )

set( VISION_SIM_SRCS VisionSimModule.cpp
  World.cpp
  Image.cpp
  VisionCorner.cpp
  ImageView.cpp
  WorldView.cpp
  Controls.cpp
  VisionSimGUI.cpp)

set( VISION_SIM_HEADERS VisionSimModule.h
  VisionSimGUI.h
  ImageView.h
  WorldView.h
  Controls.h
  VisionSimGUI.h)

Qt4_wrap_cpp( VISION_SIM_HEADERS_MOC ${VISION_SIM_HEADERS} )

add_library(
  visionsim
  STATIC
  ${VISION_SIM_SRCS}
  ${VISION_SIM_HEADERS_MOC}
  )

target_link_libraries(
  visionsim
  ${QT_LIBRARIES}
  )