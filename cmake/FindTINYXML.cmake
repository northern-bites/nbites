SET( TINYXML_DEFINITIONS "" )

INCLUDE( "${CMAKE_MODULE_PATH}/base_definitions.cmake" )

##
# Includes
##


## from ${AL_DIR}/cmakemodules/FindTINYXML.cmake
include("${AL_DIR}/cmakemodules/aldebaran.cmake")
include("${AL_DIR}/cmakemodules/libfind.cmake")

clean(TINYXML)

set(TINYXML_INCLUDE_DIR ${AL_DIR}/extern/c/src/tinyxml)
if(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${AL_DIR}/extern/c/${TARGET_ARCH}/lib/tinyxml.lib)
else(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${AL_DIR}/extern/c/${TARGET_ARCH}/lib/libtinyxml.a)
endif(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")

export(TINYXML)


