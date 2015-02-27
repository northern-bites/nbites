cmake_minimum_required( VERSION 2.8.3 )

set(SOUND_SRC Recorder.cpp)

add_library(
  sound
  STATIC
  ${SOUND_SRC} 
)

target_link_libraries(
  sound
  log
  ${ALCOMMON_LIBRARIES}
)
