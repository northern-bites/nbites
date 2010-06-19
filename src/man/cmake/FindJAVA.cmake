
##
# Includes
##

INCLUDE( ${CMAKE_MODULE_PATH}/base_definitions.cmake )

##
# Clear variables/caches
##

SET( JAVA_DEFINITIONS "" )
SET( JAVA_INCLUDE_DIR "JAVA_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)


##
# Defined package variable
##

  IF( WIN32 )
    GET_FILENAME_COMPONENT(JAVA_JDK_DIR 
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\JavaSoft\\Java Development Kit\\1.5;JavaHome]" ABSOLUTE CACHE)
    SET( JAVA_INCLUDE_DIR ${JAVA_JDK_DIR}/include ${JAVA_JDK_DIR}/include/win32)
  ELSE( WIN32 )
    IF ( APPLE )
      SET( JAVA_INCLUDE_DIR /System/Library/Frameworks/JavaVM.framework/Versions/1.5/Headers )
    ELSE ( APPLE )
      SET( JAVA_INCLUDE_DIR /usr/lib/jvm/java-6-sun/include /usr/lib/jvm/java-1.5.0-sun/include /usr/lib/jvm/java-6-sun/include/linux)
    ENDIF( APPLE )
  ENDIF( WIN32 )


IF( JAVA_INCLUDE_DIR )
  IF ( EXISTS JAVA_INCLUDE_DIR )
    SET( JAVA_FOUND TRUE )
  ENDIF ( EXISTS JAVA_INCLUDE_DIR )
ENDIF( JAVA_INCLUDE_DIR )

IF( NOT JAVA_FOUND AND JAVA_FIND_REQUIRED )
  IF( NOT JAVA_INCLUDE_DIR OR NOT EXISTS ${JAVA_INCLUDE_DIR} )
    MESSAGE( STATUS "Required include not found" )
    MESSAGE( FATAL_ERROR "Could not find JAVA include!" )
  ENDIF( NOT JAVA_INCLUDE_DIR OR NOT EXISTS ${JAVA_INCLUDE_DIR} )
ENDIF( NOT JAVA_FOUND AND JAVA_FIND_REQUIRED )

# Finally, display informations if not in quiet mode
IF( NOT JAVA_FIND_QUIETLY )
  MESSAGE( STATUS "JAVA found " )
  MESSAGE( STATUS "  includes   : ${JAVA_INCLUDE_DIR}" )
  MESSAGE( STATUS "  definitions: ${JAVA_DEFINITIONS}" )
ENDIF( NOT JAVA_FIND_QUIETLY )



MARK_AS_ADVANCED(
  JAVA_JDK_DIR
  JAVA_DEFINITIONS
  JAVA_INCLUDE_DIR
  JAVA_LIBRARIES
)
