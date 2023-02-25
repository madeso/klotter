cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(klotter CXX C)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_library(klotter::project_options ALIAS project_options)
add_library(klotter::project_warnings ALIAS project_warnings)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples)
