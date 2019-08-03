
#option(PDC_SDL2_BUILD "Build SDL2 Project" ON)
option(PDC_SDL2_DEPS_BUILD "Build SDL2 and dependencies OR sfml (default)" off)

#message(STATUS "PDC_SDL2_BUILD ......... ${PDC_SDL2_BUILD}")
message(STATUS "PDC_SDL2_DEPS_BUILD .... ${PDC_SDL2_DEPS_BUILD}")

# normalize a windows path
file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}" CMAKE_INSTALL_PREFIX)
