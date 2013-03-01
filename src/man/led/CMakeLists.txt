cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(LEDS_SRCS LedEnactorModule
  NaoLights
  Lights
  NaoRGBLight)

# Add project as a library.
add_library(
  led
  STATIC
  ${LEDS_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(led
  portals
  messages
  common
  ${ALCOMMON_LIBRARIES}
)