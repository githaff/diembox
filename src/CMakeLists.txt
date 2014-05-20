set (SOURCES
  main.c)

add_executable (${PROJECT}
  ${SOURCES})

set_target_properties (${PROJECT}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

find_package (libextopts)
include_directories(${EXTOPTS_INCLUDE_DIR})
target_link_libraries (${PROJECT} ${EXTOPTS_LIRBARY})
