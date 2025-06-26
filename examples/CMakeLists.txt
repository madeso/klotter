set(src
    main.cc
    03-lights.cc

    sample.cc
    sample.h
)

add_executable(example ${src})
set_target_properties(example PROPERTIES FOLDER "Klotter")
source_group("" FILES ${src})
cmake_source_group()
target_link_libraries(example
    PRIVATE
        klotter::klotter
        klotter::project_options
        klotter::project_warnings
)