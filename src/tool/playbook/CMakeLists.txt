cmake_minimum_required(VERSION 2.8.3)

set( PLAYBOOK_SRCS
  PlaybookCreator.cpp
  PlaybookField.cpp
  PlaybookModel.cpp
  )

set( PLAYBOOK_HEADERS
  PlaybookCreator.h
  PlaybookField.h
  PlaybookModel.h
  PlaybookConstants.h
  )

Qt4_wrap_cpp( PLAYBOOK_HEADERS_MOC ${PLAYBOOK_HEADERS} )

add_library(playbook
  STATIC
  ${PLAYBOOK_SRCS}
  ${PLAYBOOK_HEADERS_MOC}
  )

target_link_libraries(
  playbook
  tool_common
  ${QT_LIBRARIES}
  )
