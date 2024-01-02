embed(src_assets
    AS_BINARY
        textures/light_01.png
        textures/dark_01.png
        textures/container_diffuse.png
        textures/container_specular.png
        textures/matrix.jpg
        textures/cookie_01.png
        textures/glass.png
        textures/grass.png
    AS_TEXT
        klotter/render/default_shader.vert.glsl
        klotter/render/default_shader.frag.glsl
        klotter/render/pp.vert.glsl
        klotter/render/pp.invert.frag.glsl
        klotter/render/pp.grayscale.frag.glsl
        klotter/render/pp.damage.frag.glsl
        klotter/render/pp.blurv.frag.glsl
)

set(shaders
    klotter/render/default_shader.vert.glsl
    klotter/render/default_shader.frag.glsl
)

set(src_render
    klotter/render/render.cc klotter/render/render.h
    klotter/render/camera.cc klotter/render/camera.h
    klotter/render/opengl_utils.cc klotter/render/opengl_utils.h

    klotter/render/shader.cc klotter/render/shader.h
    klotter/render/shader.source.cc klotter/render/shader.source.h

    klotter/render/uniform.cc klotter/render/uniform.h
    klotter/render/texture.cc klotter/render/texture.h
    klotter/render/texture.io.h
    klotter/render/assets.cc klotter/render/assets.h

    klotter/render/vertex_layout.cc klotter/render/vertex_layout.h
    klotter/render/linebatch.cc klotter/render/linebatch.h
    klotter/render/framebuffer.cc klotter/render/framebuffer.h

    klotter/render/geom.h
    klotter/render/geom.builder.cc klotter/render/geom.builder.h
    klotter/render/geom.extract.cc klotter/render/geom.extract.h
)
    
set(src_base
    klotter/assert.cc klotter/assert.h
    klotter/colors.cc klotter/colors.h
    klotter/dependency_glad.h
    klotter/dependency_glm.h klotter/dependency_glm.cc
    klotter/dependency_sdl.cc klotter/dependency_sdl.h
    klotter/result.cc klotter/result.h
    klotter/str.cc klotter/str.h
    klotter/scurve.cc klotter/scurve.h

    klotter/klotter.cc klotter/klotter.h

    klotter/compiler_warning.h
    klotter/log.h
    klotter/cint.h
    klotter/undef_windows.h
)

set(src_pch
    klotter/pch.public.h klotter/pch.private.h
)

set(src
    ${src_render}
    ${src_base}
    ${src_pch}
    ${src_assets}
    ${shaders}
)

add_library(klotter STATIC ${src})
target_link_libraries(klotter
    PUBLIC
        external::sdl2
        external::opengl
        stb::image
        external::glm
        external::glad
        external::imgui
    PRIVATE
        external::mustache
        embed::embed
        klotter::project_options
        klotter::project_warnings
)
target_include_directories(klotter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
target_precompile_headers(klotter
    PUBLIC klotter/pch.public.h
    PRIVATE klotter/pch.private.h
)

source_group("" FILES ${src})

source_group("input" FILES ${src_input})
source_group("render" FILES ${src_render})
# source_group("base" FILES ${src_base})
source_group("pch" FILES ${src_pch})
source_group("_assets" FILES ${src_assets})
source_group("shaders" FILES ${shaders})

cmake_source_group()

add_library(klotter::klotter ALIAS klotter)