### Version control policy
###
### Extract current version from version control if present. If source
### tree is not under source control - take version string from
### .version file. Otherwise current version is v0.0.0. Both vX.X.X
### and v.X.X formats are supported.

macro (git_version_extract NAME)
  ### Extract current version from version control if present.
  ### If not - take from .version file
  if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process (COMMAND git describe
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      OUTPUT_VARIABLE GIT_TAG_VERSION)
    file (WRITE .version "${GIT_TAG_VERSION}")
  else ()
    if (EXISTS "${CMAKE_SOURCE_DIR}/.version")
      execute_process (COMMAND cat .version
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	OUTPUT_VARIABLE GIT_TAG_VERSION)
    endif ()
  endif ()

  ### Parse version
  execute_process (COMMAND echo "${GIT_TAG_VERSION}"
    COMMAND grep -E "^v[0-9][0-9]*\\.[0-9][0-9]*(-.*)?$"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE VERSION_TYPE_2)
  execute_process (COMMAND echo "${GIT_TAG_VERSION}"
    COMMAND grep -E "^v[0-9][0-9]*\\.[0-9][0-9]*\\.[0-9][0-9]*(-.*)?$"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE VERSION_TYPE_3)

  if ("${GIT_TAG_VERSION}" STREQUAL "")
    message (WARNING "No version information was found! "
      "Package should be built from correct source tree.")
    set (${NAME}_VERSION_MAJOR 0)
    set (${NAME}_VERSION_MINOR 0)
    set (${NAME}_VERSION_PATCH 0)
    set (${NAME}_VERSION "0.0.0")
    set (${NAME}_VERSION_FULL "unknown")
  else ()
    execute_process (COMMAND echo "${GIT_TAG_VERSION}"
      COMMAND sed -ne "s/^v\\([[:digit:]]*\\)\\..*/\\1/p"
      OUTPUT_VARIABLE ${NAME}_VERSION_MAJOR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND echo "${GIT_TAG_VERSION}"
      COMMAND sed -ne "s/^v[[:digit:]]*\\.\\([[:digit:]]*\\).*/\\1/p"
      OUTPUT_VARIABLE ${NAME}_VERSION_MINOR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (VERSION_TYPE_3)
      execute_process (COMMAND echo "${GIT_TAG_VERSION}"
	COMMAND sed -ne "s/^v[[:digit:]]*\\.[[:digit:]]*\\.\\([[:digit:]]*\\).*/\\1/p"
	OUTPUT_VARIABLE ${NAME}_VERSION_PATCH
	OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif ()
    execute_process (COMMAND echo "${GIT_TAG_VERSION}"
      COMMAND sed -ne "s/[^-]*-\\(.*\\)/\\1/p"
      OUTPUT_VARIABLE ${NAME}_VERSION_DIRT
      OUTPUT_STRIP_TRAILING_WHITESPACE)

    if (VERSION_TYPE_3)
      set (${NAME}_VERSION "${${NAME}_VERSION_MAJOR}.${${NAME}_VERSION_MINOR}.${${NAME}_VERSION_PATCH}")
    else ()
      set (${NAME}_VERSION "${${NAME}_VERSION_MAJOR}.${${NAME}_VERSION_MINOR}")
    endif ()

    if (${NAME}_VERSION_DIRT STREQUAL "")
      set (${NAME}_VERSION_FULL "${${NAME}_VERSION}")
    else ()
      set (${NAME}_VERSION_FULL "${${NAME}_VERSION}-${${NAME}_VERSION_DIRT}")
    endif ()
  endif ()

  message (STATUS "Building version: ${EMBOX_VERSION_FULL}")
endmacro (git_version_extract)
