# Download and build all of the GTEST crap

# Only update gtest if the user is missing it.
# Make three processes because apparently commands are NOT executed sequentially.
if( NOT EXISTS ${NBITES_DIR}/lib/gtest-1.6.0 )
  execute_process( COMMAND wget http://googletest.googlecode.com/files/gtest-1.6.0.zip
    WORKING_DIRECTORY ${NBITES_DIR}/lib
    )
  execute_process( COMMAND unzip -q gtest-1.6.0.zip
    WORKING_DIRECTORY ${NBITES_DIR}/lib
    )
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
