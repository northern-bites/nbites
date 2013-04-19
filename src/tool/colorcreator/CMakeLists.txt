cmake_minimum_required(VERSION 2.8.3)

set( ColorCreator_srcs
  ColorTable.cpp
  ColorTableCreator.cpp
  Stats.cpp
  )

set(ColorCreator_headers
    ColorTableCreator.h
)

Qt4_wrap_cpp(ColorCreator_headers_moc ${ColorCreator_headers})

add_library(
    colorcreator
    STATIC
    ${ColorCreator_srcs}
    ${ColorCreator_headers_moc}
    )

target_link_libraries(
  colorcreator
  portals
  messages
  image
  ${QT_LIBRARIES}
  )
