############

cmake_minimum_required(VERSION 2.8.0)

# set up sub projects and includes

include_directories( . ) #current directory
add_subdirectory(man)
# this pulls all the include dependencies of man so that the qtool can inherit them
get_directory_property(man_includes DIRECTORY man INCLUDE_DIRECTORIES)
include_directories( ${man_includes} )
add_subdirectory(qtool)
