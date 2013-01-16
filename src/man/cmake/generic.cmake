
# Set options that are constrained by building straight
set( OFFLINE ON )

# Find the NBITES_DIR
include(${CMAKE_CURRENT_LIST_DIR}/FindNBITES_DIR.cmake)

# This is the version the robots use
set( CMAKE_USE_PYTHON_VERSION 2.6 )
