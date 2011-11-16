############

cmake_minimum_required(VERSION 2.8.0)

# set up sub projects and includes

set( NBITES_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
set( MAN_SRC_DIR ${NBITES_SRC_DIR}/man )
set( QTOOL_SRC_DIR ${NBITES_SRC_DIR}/qtool )

include_directories( . ) #current directory
add_subdirectory(man)
# this pulls all the include dependencies of man so that the qtool can inherit them
get_directory_property(man_includes DIRECTORY man INCLUDE_DIRECTORIES)
include_directories( ${man_includes} )
add_subdirectory(qtool)
