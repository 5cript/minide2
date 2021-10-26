# Version check
cmake_minimum_required (VERSION 3.8)

# Project
project(minideback)

add_executable(
    minideback
    "backend/main.cpp"
    "backend/filesystem/directory_cache.cpp"
    "backend/filesystem/filesystem.cpp"
    "backend/filesystem/home_directory.cpp"
    "backend/filesystem/relations.cpp"
    "backend/settings/environment.cpp"
    "backend/log.cpp"
    "backend/public_settings.cpp"
    "backend/termination_handler.cpp"
)

target_link_libraries(minideback 
    PUBLIC 
        project_settings
    PRIVATE
        special_paths
        attender
        ${Boost_LIBRARIES}
        -lbacktrace
)
target_include_directories(minideback 
    PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/include>"
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)