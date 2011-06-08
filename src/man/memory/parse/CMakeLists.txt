

############################ Version
# Check cMake version
cmake_minimum_required( VERSION 2.6.0 )


############################ FIND PACKAGE
# Libraries specifics.
# cmake packages we use
FIND_PACKAGE( Protobuf REQUIRED )
#FIND_PACKAGE( ZLIB )

############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${PROTOBUF_INCLUDE_DIRS}/
                     )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( PARSER_SRCS MessageParser.cpp
                 ImageParser.cpp
                 )

############################ Add sources
# Add static library target
add_library(
  ${PARSER_TARGET}
  STATIC
  ${PARSER_SRCS}
  )

target_link_libraries(
    ${PARSER_TARGET}
    ${PROTOBUF_LIBRARIES}
    )
