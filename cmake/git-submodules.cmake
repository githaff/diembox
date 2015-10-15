### Git submodules management
###
### Fetch submodules registered in this project for further use as
### subprojects

function (git_fetch_submodules)
  ### Check if sumbodules exist at all
  if (NOT EXISTS "${PROJECT_SOURCE_DIR}/.gitmodules")
    message (WARNING "No git submodules are present in this git repository")
    return ()
  endif (NOT EXISTS "${PROJECT_SOURCE_DIR}/.gitmodules")

  ### Fetch modules supressing std output
  message (STATUS "Fetching submodules")
  execute_process(
    COMMAND             git submodule update --init --recursive
    WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
    OUTPUT_QUIET)
endfunction (git_fetch_submodules)

