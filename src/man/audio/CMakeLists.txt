cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(LEDS_SRCS
  AudioEnactorModule
  ALSpeech)

# Add project as a library.
add_library(
  audio
  STATIC
  ${AUDIO_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(audio
  portals
  messages
  ${ALCOMMON_LIBRARIES}
)