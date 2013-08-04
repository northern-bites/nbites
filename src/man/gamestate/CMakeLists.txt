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

# GTEST
if (OFFLINE)
  nbites_add_test(GameStateTest GameStateTest.cpp
    gamestate
    )
endif()
