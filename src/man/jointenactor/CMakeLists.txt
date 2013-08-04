cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled. 
set(JOINT_ENACTOR_SRCS JointEnactorModule)

# Add project as a library. 
add_library(
  jointenactor
  STATIC
  ${JOINT_ENACTOR_SRCS}
)

# Link project to appropriate libraries. 
target_link_libraries(jointenactor 
  portals
  messages
  ${ALCOMMON_LIBRARIES}
)