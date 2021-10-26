cmake_minimum_required(VERSION 3.21)

project(minide2 VERSION 0.1.0)

include(cmake/options.cmake)
include(cmake/project_settings.cmake)

add_subdirectory(attender)
add_subdirectory(automata)
add_subdirectory(debugger-interface)
#broken, replace or contribute:
#add_subdirectory(filewatch)
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_subdirectory(lldb-mi)
endif()
#dont use anylonger, use simdjson or nlohmann
#add_subdirectory(SimpleJSON)
add_subdirectory(special-paths)
add_subdirectory(tiny-process-library)
#use openssh instead
#include(hash-library)

add_subdirectory(backend/src)