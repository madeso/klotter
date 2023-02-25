add_executable(cube 01-cube/01-cube.cc)
target_link_libraries(cube PRIVATE klotter::klotter klotter::project_options klotter::project_warnings)
