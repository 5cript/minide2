# Version check
cmake_minimum_required (VERSION 3.21)

find_package(PkgConfig REQUIRED)
pkg_check_modules(v8_pkg REQUIRED IMPORTED_TARGET v8)
pkg_check_modules(v8_base_pkg REQUIRED IMPORTED_TARGET v8_libbase)
pkg_check_modules(v8_platform_pkg REQUIRED IMPORTED_TARGET v8_libplatform)

find_file(v8_snapshot_bin snapshot_blob.bin REQUIRED)
add_custom_target(
    v8_snapshot_copy
    COMMAND ${CMAKE_COMMAND} -E copy ${v8_snapshot_bin} "${CMAKE_BINARY_DIR}/bin/snapshot_blob.bin"
)

add_library(v8 INTERFACE)
target_link_libraries(v8 INTERFACE PkgConfig::v8_pkg PkgConfig::v8_base_pkg PkgConfig::v8_platform_pkg)
add_dependencies(v8 v8_snapshot_copy)