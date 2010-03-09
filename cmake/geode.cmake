

############################ ALDEBARAN DIRECTORY
# Ensure the AL_DIR variable is set

IF( "x$ENV{AL_DIR}x" STREQUAL "xx")
    SET( AL_DIR "/usr/local/nao-1.4" )
  SET( ENV{AL_DIR} ${AL_DIR} )
  MESSAGE( STATUS
    "Environment variable AL_DIR was not set, reseting to default ${AL_DIR}!" )
ELSE( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR $ENV{AL_DIR} )
ENDIF( "x$ENV{AL_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${AL_DIR} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to Nao directory, configuration halted."
    )
ENDIF( NOT EXISTS ${AL_DIR} )

############################# CROSS-COMPILATION VARIABLES
# Set the variable for the cross-compilation directory, cmake variables

INCLUDE( CMakeForceCompiler )

SET( OE_PREFIX "i586-linux" )
SET( STAGING_PREFIX "geode-linux" )
SET( GCC_VERSION "4.3.3" )
SET( OE_CROSS_DIR "${AL_DIR}/crosstoolchain" )

SET( CMAKE_CROSSCOMPILING   TRUE  )
SET( CMAKE_SYSTEM_NAME      Linux )
SET( CMAKE_SYSTEM_VERSION   1     )
SET( CMAKE_SYSTEM_PROCESSOR geode )

SET( X86_BIN_PREFIX "${OE_PREFIX}-" )
SET( X86_BIN_DIR "${OE_CROSS_DIR}/cross/geode/bin" )

SET ( CMAKE_INSTALL_NAME_TOOL "${OE_CROSS_DIR}/staging/${STAGING-PREFIX}/usr/lib/install_name_tool")
SET( CMAKE_C_COMPILER "${X86_BIN_DIR}/${X86_BIN_PREFIX}gcc" )
SET( CMAKE_CXX_COMPILER "${X86_BIN_DIR}/${X86_BIN_PREFIX}g++" )
CMAKE_FORCE_C_COMPILER( "${X86_BIN_DIR}/${X86_BIN_PREFIX}gcc" GNU )
CMAKE_FORCE_CXX_COMPILER( "${X86_BIN_DIR}/${X86_BIN_PREFIX}g++" GNU )

SET( CMAKE_FIND_ROOT_PATH ${OE_CROSS_DIR}/staging/${OE_PREFIX} )
SET( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
SET( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE FIRST )
SET( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )


############################# MISC
# Set other miscellaneous variables

SET( X86_GCCLIB_DIR "${OE_CROSS_DIR}/cross/geode/lib/gcc/${OE_PREFIX}/${GCC_VERSION}/" )
SET( X86_GLIBC_DIR "${OE_CROSS_DIR}/cross/geode/${OE_PREFIX}/lib/" )
SET( X86_LDLINUXSO_DIR "${OE_CROSS_DIR}/staging/${STAGING_PREFIX}/lib/" )
SET( X86_INCLUDE_DIR "${OE_CROSS_DIR}/staging/${STAGING_PREFIX}/usr/include/" )
SET( X86_CPINCLUDE_DIR "${OE_CROSS_DIR}/staging/${STAGING_PREFIX}/usr/include/c++/" )
SET( X86_GCPINCLUDE_DIR "${OE_CROSS_DIR}/staging/${STAGING_PREFIX}/usr/include/c++/${OE_PREFIX}/" )
SET( X86_GINCLUDE_DIR "${OE_CROSS_DIR}/cross/geode/lib/gcc/${OE_PREFIX}/${GCC_VERSION}/include/" )

SET( X86_CRT "${X86_GCCLIB_DIR}/crtbegin.o ${X86_GCCLIB_DIR}/crtbeginS.o" )


############################# FLAGS
# Set the compile and linker flags

SET( CMAKE_C_FLAGS "--sysroot ${OE_CROSS_DIR}/staging/${OE_PREFIX}/ -I${X86_INCLUDE_DIR} -I${X86_GINCLUDE_DIR} -I${X86_CPINCLUDE_DIR} -I${X86_GCPINCLUDE_DIR} -march=geode" )
SET( CMAKE_CXX_FLAGS "--sysroot ${OE_CROSS_DIR}/staging/${OE_PREFIX}/ -I${X86_INCLUDE_DIR} -I${X86_GINCLUDE_DIR} -I${X86_CPINCLUDE_DIR} -I${X86_GCPINCLUDE_DIR} -march=geode" )
SET( CMAKE_EXE_LINKER_FLAGS "-Wl,--sysroot,${OE_CROSS_DIR}/staging/${OE_PREFIX}/ -lgcc -L${X86_GLIBC_DIR} -lc -lstdc++ -ldl" )

INCLUDE("${OE_CROSS_DIR}/toolchain-geode.cmake")