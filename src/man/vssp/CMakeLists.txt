## vssp ##

project( vssp_tool )
cmake_minimum_required(VERSION 2.8.0)

## dependencies ##

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

## sources ##

set( vssp_tool_sources main.cpp
  )

set( vssp_tool_headers
  )

include_directories(.
  ${CMAKE_CURRENT_BINARY_DIR}
  )

#Qt_wrap_cpp(vssp_tool_headers_moc ${vssp_tool_headers})

add_executable( vssp_tool
  ${vssp_tool_sources}
  ${vssp_tool_headers_moc}
  )

target_link_libraries(
  vssp_tool
  ${QT_LIBRARIES}
)