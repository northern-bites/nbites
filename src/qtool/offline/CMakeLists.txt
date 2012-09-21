
############ offline

project( offline )
cmake_minimum_required(VERSION 2.8.3)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( offline_srcs OfflineViewer.cpp )

set( offline_headers OfflineViewer.h )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(offline_headers_moc ${offline_headers})
Qt4_wrap_ui(offline_forms_headers ${offline_forms})

add_library(offline
    STATIC
    ${offline_srcs}
    ${offline_headers_moc}
    ${offline_forms_headers} )

target_link_libraries(
  offline
  viewer
  manpreloader
  ${QT_LIBRARIES}
  )

# Copy files from source directory to destination directory, substituting any
# variables.  Create destination directory if it does not exist.
# http://stackoverflow.com/questions/697560/how-to-copy-directory-from-source-tree-to-binary-tree

macro(copy_py_sources srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB_RECURSE templateFiles RELATIVE ${srcDir} ${srcDir}/*.py)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(copy_py_sources)

add_dependencies( offline man )

copy_py_sources( ${MAN_SRC_DIR} ${CMAKE_BINARY_DIR}/man )