# Version check
cmake_minimum_required (VERSION 3.8)

# Project
project(minideback)

add_executable(
    minideback
    "backend/main.cpp"
    "backend/log.cpp"
    "backend/public_settings.cpp"
    "backend/termination_handler.cpp"

    # Filesystem
    "backend/filesystem/directory_cache.cpp"
    "backend/filesystem/filesystem.cpp"
    "backend/filesystem/home_directory.cpp"
    "backend/filesystem/jail.cpp"
    "backend/filesystem/directory_contents.cpp"

    # Server
    "backend/server/backend_control.cpp"
    "backend/server/frontend_user_session.cpp"
    "backend/server/writer.cpp"
    "backend/server/stream/stream_parser.cpp"
    "backend/server/stream/subscription.cpp"
    "backend/server/stream/dispatcher.cpp"
    "backend/server/api/user.cpp"
    "backend/server/api/workspace.cpp"

    # Persistence And Settings
    "backend/settings/environment.cpp"

    # Plugin System
    "backend/plugin_system/api/console.cpp"
    "backend/plugin_system/plugin.cpp"
)

find_package(Threads REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(minideback 
    PUBLIC 
        project_settings
    PRIVATE
        special_paths
        attender
        v8pp
        v8
        fmt::fmt
        v8wrap
        ${Boost_LIBRARIES}
        -lbacktrace
        Threads::Threads
)
target_include_directories(minideback 
    PUBLIC 
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/backend/include>"
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)