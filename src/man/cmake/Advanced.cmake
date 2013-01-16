# Marks all options that should be hidden when we "make cross/straight" as
#  advanced so that the list is not overwhelming
cmake_minimum_required( VERSION 2.8.3 )

mark_as_advanced(
  CCACHE
  CMAKE_INSTALL_PREFIX
  CMAKE_TOOLCHAIN_FILE
  I_AM_A_ROBOT
)