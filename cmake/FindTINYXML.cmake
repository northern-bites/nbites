SET( TINYXML_DEFINITIONS "" )

INCLUDE( "${CMAKE_MODULE_PATH}/base_definitions.cmake" )

##
# Includes
##


## from ${AL_DIR}/cmakemodules/FindTINYXML.cmake

set(TINYXML_INCLUDE_DIR ${OE_CROSS_DIR}/staging/geode-linux/usr/include/tinyxml)
if(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${OE_CROSS_DIR}/staging/geode-linux/usr/lib/tinyxml.lib)
else(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${OE_CROSS_DIR}/staging/geode-linux/usr/lib/libtinyxml.a)
endif(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")


