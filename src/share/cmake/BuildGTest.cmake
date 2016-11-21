# Download and build all of the GTEST crap

# Only update gtest if the user is missing it.
# Make seperate processes because apparently commands are NOT executed sequentially.
if( NOT EXISTS ${NBITES_DIR}/lib/gtest-1.6.0 )
  # Download gtest from Github
  execute_process( COMMAND wget https://github.com/google/googletest/archive/release-1.6.0.zip
    WORKING_DIRECTORY ${NBITES_DIR}/lib
    )
  # Rename the file to what our build process expects
  execute_process( COMMAND mv ${NBITES_DIR}/lib/release-1.6.0.zip ${NBITES_DIR}/lib/gtest-1.6.0.zip )
  # Remove the directory if it already exists so we don't get overwriting problems
  execute_process( COMMAND rm -rf ${NBITES_DIR}/lib/googletest-release-1.6.0/ )
  # Unzip the zip file
  execute_process( COMMAND unzip -q gtest-1.6.0.zip
    WORKING_DIRECTORY ${NBITES_DIR}/lib
    )
  # Rename the folder that now exists
  execute_process( COMMAND mv ${NBITES_DIR}/lib/googletest-release-1.6.0 ${NBITES_DIR}/lib/gtest-1.6.0 )
  # Delete the zip file
  execute_process( COMMAND rm gtest-1.6.0.zip
    WORKING_DIRECTORY ${NBITES_DIR}/lib
    )

endif()

# Build GTest by adding as a subdirectory, but build it in gtest
add_subdirectory( ${NBITES_DIR}/lib/gtest-1.6.0 ${CMAKE_CURRENT_BINARY_DIR}/gtest)

mark_as_advanced( gtest_build_samples
  gtest_build_tests
  gtest_disable_pthreads
  gtest_force_shared_crt
)

enable_testing()
set(CTEST_OUTPUT_ON_FAILURE TRUE)

include_directories( ${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} )

## Macro for running our tests
# testname will be the executable
# testfile is the cpp file containing gtests
# any other parameters will be linked to the executable
macro( nbites_add_test testname testfile)
  add_executable( ${testname} ${CMAKE_CURRENT_LIST_DIR}/test/${testfile} )
  target_link_libraries(
    ${testname}
    ${ARGN}
#    ${GTEST_BOTH_LIBRARIES} Not working... don't know why...
    gtest
    gtest_main
    )
  add_test(${testname} ${testname})
endmacro()
