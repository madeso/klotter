add_executable(cube 01-cube/01-cube.cc)
source_group("" FILES 01-cube/01-cube.cc)
cmake_source_group()
target_link_libraries(cube PRIVATE klotter::klotter klotter::project_options klotter::project_warnings)
