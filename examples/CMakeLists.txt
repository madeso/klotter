function(add_sample name)
    add_executable(${name} ${name}.cc)
    source_group("" FILES ${name}.cc)
    cmake_source_group()
    target_link_libraries(${name}
        PRIVATE
            klotter::klotter
            klotter::project_options
            klotter::project_warnings
    )
endfunction()

add_sample(01-cube)
add_sample(02-scene)