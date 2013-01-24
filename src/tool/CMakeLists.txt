# Main CMake file for the Tool!
project( tool )
cmake_minimum_required(VERSION 2.8.3)

# Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# Compiler flags--might not need these, Lizzie?

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

# Sources

set( tool_srcs
  EmptyQTool.cpp
  )

set( tool_headers
  EmptyQTool.h
)

# Qt4 goodness

Qt4_wrap_cpp(tool_headers_moc ${tool_headers})

# The main executable
add_executable( tool
  ${tool_srcs}
  ${tool_headers_moc} )

target_link_libraries(
  tool
  ${QT_LIBRARIES}
  )
