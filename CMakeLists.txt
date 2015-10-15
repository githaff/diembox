cmake_minimum_required (VERSION 2.8)

### Name of whole complex
set (PROJECT "embox")
project (${PROJECT})

# Set compile parameters
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -O0 -g3 -ggdb")
# Set additional warnings
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-long-long -pedantic")

### Project directories
## CMake modules
set (CMAKE_MODULE_PATH "cmake")
set (SOURCES_DIR "${CMAKE_SOURCE_DIR}/src")
set (BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
if (NOT EXISTS "${BUILD_DIR}")
  file (MAKE_DIRECTORY "${BUILD_DIR}")
endif()

## Installation paths if not specified
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
  set (CMAKE_INSTALL_BINDIR "bin")
endif (NOT DEFINED CMAKE_INSTALL_BINDIR)

add_subdirectory (${SOURCES_DIR})

### Determine package version
include (git-version)
include (git-submodules)

git_version_extract ()
git_submodules_fetch ()

### Packaging setup
include (InstallRequiredSystemLibraries)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Embox utilities toolset")
set (CPACK_PACKAGE_VENDOR "haff")
set (CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
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
