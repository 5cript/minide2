cmake_minimum_required(VERSION 3.21)

project(minide2 VERSION 0.1.0)

include(cmake/options.cmake)
include(cmake/project_settings.cmake)
include(cmake/packages.cmake)
include(cmake/v8.cmake)

#set(V8PP_HEADER_ONLY ON)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_subdirectory(lldb-mi)
endif()
add_subdirectory(v8pp)
add_subdirectory(attender)
add_subdirectory(automata)
add_subdirectory(debugger-interface)
add_subdirectory(special-paths)

add_subdirectory(v8wrap/src)
add_subdirectory(backend/src)

# use boost process
#add_subdirectory(tiny-process-library)

#broken, replace or contribute:
#add_subdirectory(filewatch)
