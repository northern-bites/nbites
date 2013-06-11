cmake_minimum_required( VERSION 2.8.3 )

# Set source files here that need to be compiled.
set( GAME_STATE_SRCS GameStateModule
  )

# Add project as a library.
add_library(
  gamestate
  STATIC
  ${GAME_STATE_SRCS}
  )

# Link project to appropriate libraries.
target_link_libraries(
  gamestate
  support
  portals
  messages
  )

# nbites_add_test(GameState GameStateTest.cpp
#   gamestate
#   comm
#   guardian
#   behavior
#   support
#   portals
#   messages
# )

add_executable(GameStateTest
  ${CMAKE_CURRENT_LIST_DIR}/test/GameStateTest.cpp
  GameStateModule.cpp
)
target_link_libraries(
  GameStateTest
  ${G_TEST_BOTH_LIBRARIES}
  gamestate
  comm
  guardian
  behavior
  support
  portals
  messages
)

#GTEST_ADD_TESTS????

add_test(GameStateTest GameStateTest)