set(src_input
    three/input/axis.cc three/input/axis.h
    three/input/key.cc three/input/key.h
)

set(src_render
    three/render/camera.cc three/render/camera.h
)
    
set(src_base
    three/assert.cc three/assert.h
    three/colors.cc three/colors.h
    three/dependency_glad.h
    three/dependency_glm.h three/dependency_glm.cc
    three/dependency_opengl.cc three/dependency_opengl.h
    three/dependency_sdl.cc three/dependency_sdl.h
    three/result.cc three/result.h
    three/str.cc three/str.h
    three/windows.sdl.convert.cc three/windows.sdl.convert.h
)

set(src_pch
    three/pch.public.h three/pch.private.h
)

set(src
    ${src_input}
    ${src_empty}
    ${src_render}
    ${src_base}
    ${src_pch}
)

add_library(three STATIC ${src})
target_link_libraries(three
    PUBLIC
        external::sdl2
        external::opengl
        stb::image
        external::glm
        external::glad
    PRIVATE
        three::project_options
        three::project_warnings
)
target_include_directories(three
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_precompile_headers(three
    PUBLIC three/pch.public.h
    PRIVATE three/pch.private.h
)

source_group("" FILES ${src})

source_group("input" FILES ${src_input})
source_group("empty" FILES ${src_empty})
source_group("render" FILES ${src_render})
source_group("base" FILES ${src_base})
source_group("pch" FILES ${src_pch})

add_library(three::three ALIAS three)