cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(theecpp CXX C)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples)
