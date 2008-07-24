

############################ OPEN-R DIRECTORY
# Ensure the OPEN_R_SDK_ROOT variable is set

IF( "x$ENV{OPEN_R_SDK_ROOT}x" STREQUAL "xx" )
    SET( OPEN_R_SDK_ROOT "/usr/local/OPEN_R_SDK" )
    SET( ENV{OPEN_R_SDK_ROOT} ${OPEN_R_SDK_ROOT} )
    MESSAGE( STATUS 
        "Environment variable OPEN_R_SDK_ROOT was not set, reseting to default ${OPEN_R_SDK_ROOT}!"
	)
ELSE( "x$ENV{OPEN_R_SDK_ROOT}x" STREQUAL "xx" )
    SET( OPEN_R_SDK_ROOT $ENV{OPEN_R_SDK_ROOT} )
ENDIF( "x$ENV{OPEN_R_SDK_ROOT}x" STREQUAL "xx" )


############################# CROSS-COMPILATION VARIABLES
# Set the variable for the cross-compilation directory, cmake variables
SET( OE_PREFIX "mipsel-linux" )
SET( OE_CROSS_DIR "${OPEN_R_SDK_ROOT}" )

SET( CMAKE_CROSSCOMPILING TRUE )
SET( CMAKE_SYSTEM_NAME    Linux )
SET( CMAKE_SYSTEM_VERSION 1 )

SET( X86_BIN_DIR "${OE_CROSS_DIR}/bin" )
SET( X86_BIN_PREFIX "${OE_PREFIX}-" )

SET( CMAKE_C_COMPILER "${X86_BIN_DIR}/${X86_BIN_PREFIX}gcc" )
SET( CMAKE_CXX_COMPILER "${X86_BIN_DIR}/${X86_BIN_PREFIX}g++" )
SET( CMAKE_AR "${X86_BIN_DIR}/${X86_BIN_PREFIX}ar" )
SET( CMAKE_RANLIB "${X86_BIN_DIR}/${X86_BIN_PREFIX}ranlib" )
SET( CMAKE_LINKER "${OE_CROSS_DIR}/OPEN_R/bin/mkbin" )
SET( CMAKE_STRIP "${X86_BIN_DIR}/${X86_BIN_PREFIX}strip" )

SET(CMAKE_FIND_ROOT_PATH  ${OE_CROSS_DIR} )
#SET( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


############################# MISC
# Set other miscellaneous variables

SET( X86_GCCLIB_DIR "${OE_CROSS_DIR}/lib/gcc-lib/${OE_PREFIX}/3.2.2" )
SET( X86_GLIBC_DIR "${OE_CROSS_DIR}/${OE_PREFIX}/lib" )
SET( X86_LDLINUXSO_DIR "${OE_CROSS_DIR}/OPEN_R/lib" )
SET( X86_INCLUDE_DIR "${OE_CROSS_DIR}/OPEN_R/usr/include" )
#SET( X86_INCLUDE2_DIR "${OE_CROSS_DIR}/OPEN_R/include" )
SET( X86_CPINCLUDE_DIR "${OE_CROSS_DIR}/include/c++/3.2.2" )
SET( X86_GCPINCLUDE_DIR "${OE_CROSS_DIR}/include/c++/3.2.2/${OE_PREFIX}" )
SET( X86_GINCLUDE_DIR "${OE_CROSS_DIR}/lib/gcc-lib/${OE_PREFIX}/3.2.2/include/" )

SET( INCLUDE1_DIR "${OE_CROSS_DIR}/OPEN_R/include" )
SET( INCLUDE2_DIR "${OE_CROSS_DIR}/OPEN_R/include/MCOOP" )
SET( INCLUDE3_DIR "${OE_CROSS_DIR}/OPEN_R/include/R4000" )

SET( X86_CRT "${X86_GCCLIB_DIR}/crtbegin.o ${X86_GCCLIB_DIR}/crtbeginS.o" )


############################# FLAGS
# Set the compile and linker flags

SET( CMAKE_CXX_FLAGS "-isystem${INCLUDE1_DIR} -isystem${INCLUDE2_DIR} -isystem${INCLUDE3_DIR} -g" )
SET( CMAKE_EXE_LINKER_FLAGS "-p ${OE_CROSS_DIR} -lObjectComm -lOPENR -lInternet -lantMCOOP -lERA201D1" )
SET( CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" )


