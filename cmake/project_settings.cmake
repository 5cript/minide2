# Version check
cmake_minimum_required (VERSION 3.21)

# Default Release Build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WARNINGS
    -Wall
    -Wextra
    -Wshadow
    -Wnon-virtual-dtor
    -Wformat=2
    -Wpedantic
)

add_library(project_settings INTERFACE)
target_compile_features(project_settings INTERFACE cxx_std_20)
target_compile_options(project_settings INTERFACE "$<$<CONFIG:DEBUG>:-fexceptions;-g;${WARNINGS}>")
target_compile_options(project_settings INTERFACE "$<$<CONFIG:RELEASE>:-fexceptions;-O3;${WARNINGS};-Werror>")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(project_settings INTERFACE "libc++")
else()
  target_link_libraries(project_settings INTERFACE "-lstdc++ -lgcc")
endif()