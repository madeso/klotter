add_executable(cube 01-cube/01-cube.cc)
target_link_libraries(cube PRIVATE three::three three::project_options three::project_warnings)
