# Copyright 2023-2025 hrzlgnm
# SPDX-License-Identifier: MIT-0

include(cmake/folders.cmake)

include(CTest)
add_subdirectory(test)

add_custom_target(run-exe COMMENT runs executable COMMAND cappuchin_exe VERBATIM)
add_dependencies(run-exe cappuchin_exe)

add_folders(Project)
