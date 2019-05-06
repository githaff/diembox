set (SOURCES
  common.c
  timestream.c
  tobin.c
  tobin-calc.c
  cat.c
  help.c
  main.c)
set (HEADERS
  common.h
  timestream.h
  tobin.h
  main-info.h)

configure_file(config.h.in config.h @ONLY)

add_executable (${PROJECT}
  ${SOURCES} ${HEADERS})

set_target_properties (${PROJECT}
  PROPERTIES
  OUTPUT_NAME ${PROJECT})

find_package (extopts)
include_directories (
  ${EXTOPTS_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries (${PROJECT} ${EXTOPTS_LIBRARY_STATIC})
target_link_libraries (${PROJECT} rt)

### Install rules
install (TARGETS ${PROJECT} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
