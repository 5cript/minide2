# Version check
cmake_minimum_required (VERSION 3.8)

# Project
project(minideback)

# Add files

# Add library
add_executable(
    minideback
    "backend/main.cpp"
)

# Compiler Options
target_link_libraries(minideback PUBLIC project_settings)
