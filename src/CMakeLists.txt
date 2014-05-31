set (SOURCES
  help.c
  main.c)
set (HEADERS
  main-info.h)

configure_file(config.h.in config.h @ONLY)

add_executable (${PROJECT}
  ${SOURCES} ${HEADERS})

set_target_properties (${PROJECT}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

find_package (extopts)
include_directories(${EXTOPTS_INCLUDE_DIR})
target_link_libraries (${PROJECT} ${EXTOPTS_LIBRARY})
