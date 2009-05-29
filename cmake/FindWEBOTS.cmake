

SET(WEBOTS_DIR $ENV{WEBOTS_HOME})
SET(WEBOTS_INCLUDE_DIR "${WEBOTS_DIR}/include/controller/c"
                       "${WEBOTS_DIR}/include/")


IF( "${WEBOTS_DIR}" STREQUAL "")
  MESSAGE(ERROR "Unable to set WEBOTS DIR. Make sure to set WEBOTS_HOME in .bashrc or .profile")
ENDIF(  "${WEBOTS_DIR}" STREQUAL "")


SET(WEBOTS_DEFINITIONS -DWEBOTS_BACKEND)

#This needs to get set for other platforms besides Linux, eventually
SET(WEBOTS_LIBRARIES "${WEBOTS_DIR}/lib/libController.so")

MARK_AS_ADVANCED( WEBOTS_INCLUDE_DIR )