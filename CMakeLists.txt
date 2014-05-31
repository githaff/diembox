cmake_minimum_required (VERSION 2.8)


### Name of whole complex
set (PROJECT "embox")
project (${PROJECT})

# Set compile parameters
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O0 -g")
# Set additional warnings
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic")

### Extract current version from version control if present.
### If not - take from .version file
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
  execute_process (COMMAND git describe
    OUTPUT_VARIABLE GIT_TAG_VERSION)
  file (WRITE .version "${GIT_TAG_VERSION}")
else ()
  if (EXISTS "${CMAKE_SOURCE_DIR}/.version")
    execute_process (COMMAND cat .version
      OUTPUT_VARIABLE GIT_TAG_VERSION)
  endif ()
endif ()
## Parse version
if ("${GIT_TAG_VERSION}" STREQUAL "")
  message (WARNING "No version information was found! "
    "Package should be built from correct source tree.")
  set (EMBOX_VERSION_MAJOR 0)
  set (EMBOX_VERSION_MINOR 0)
  set (EMBOX_VERSION "0.0")
  set (EMBOX_VERSION_FULL "unknown")
else ()
  execute_process (COMMAND echo "${GIT_TAG_VERSION}"
    COMMAND sed -ne "s/^v\\([[:digit:]]\\)\\..*/\\1/p"
    OUTPUT_VARIABLE EMBOX_VERSION_MAJOR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND echo "${GIT_TAG_VERSION}"
    COMMAND sed -ne "s/^v[[:digit:]]\\.\\([[:digit:]]\\).*/\\1/p"
    OUTPUT_VARIABLE EMBOX_VERSION_MINOR
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process (COMMAND echo "${GIT_TAG_VERSION}"
    COMMAND sed -ne "s/.*-\\(.*\\)/\\1/p"
    OUTPUT_VARIABLE EMBOX_VERSION_DIRT
    OUTPUT_STRIP_TRAILING_WHITESPACE)

  set (EMBOX_VERSION "${EMBOX_VERSION_MAJOR}.${EMBOX_VERSION_MINOR}")
  if (EMBOX_VERSION_DIRT STREQUAL "")
    set (EMBOX_VERSION_FULL "${EMBOX_VERSION}")
  else ()
    set (EMBOX_VERSION_FULL "${EMBOX_VERSION}-${EMBOX_VERSION_DIRT}")
  endif ()
endif ()


### Project directories
## Build product directories
set (BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
## Sources
set (SRC_DIR "${CMAKE_SOURCE_DIR}/src")

if (NOT EXISTS "${BUILD_DIR}")
  file (MAKE_DIRECTORY "${BUILD_DIR}")
endif()

## Installation paths if not specified
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
  set (CMAKE_INSTALL_BINDIR "bin")
endif (NOT DEFINED CMAKE_INSTALL_BINDIR)

add_subdirectory (${SRC_DIR})

### Packaging setup
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Embox utilities toolset")
set (CPACK_PACKAGE_VENDOR "haff")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set (CPACK_PACKAGE_VERSION_MAJOR "${EMBOX_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${EMBOX_VERSION_MINOR}")
set (CPACK_SOURCE_GENERATOR "TGZ")
set (CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${EMBOX_VERSION_FULL}")
set (CPACK_SOURCE_IGNORE_FILES 
  ".*~"
  "#.*#"
  ".*.tar.gz"
  "/.git"
  "/build"
  "/_CPack_Packages"
  ".gitignore"
  "CMakeCache.txt"
  "CMakeFiles"
  "CPackConfig.cmake"
  "CPackSourceConfig.cmake"
  "Makefile"
  "cmake_install.cmake"
  "install_manifest.txt"
)
include (CPack)
## Add alias to default 'dist' target
add_custom_target (dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
