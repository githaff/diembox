set (SOURCES
  main.c)

add_executable (${PROJECT}
  ${SOURCES})

set_target_properties (${PROJECT}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
