cmake_minimum_required (VERSION 2.6)

project (FrostCore)

SET(CMAKE_BINARY_DIR ${CMAKE_BUILD_DIR}/bin)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(source)

unset(MySQL_DIR)