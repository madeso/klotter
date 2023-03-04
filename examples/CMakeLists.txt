function(add_sample name)
    
endfunction()

add_sample(01-cube)
add_sample(02-scene)

set(src
    main.cc
    01-cube.cc
    02-scene.cc
)

add_executable(example ${src})
source_group("" FILES ${src})
cmake_source_group()
target_link_libraries(example
    PRIVATE
        klotter::klotter
        klotter::project_options
        klotter::project_warnings
)