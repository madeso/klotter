set(shaders
    klotter/render/shaders/default_shader.vert.glsl
    klotter/render/shaders/default_shader.frag.glsl
    klotter/render/shaders/pp.vert.glsl
    klotter/render/shaders/pp.invert.frag.glsl
    klotter/render/shaders/pp.grayscale.frag.glsl
    klotter/render/shaders/pp.damage.frag.glsl
    klotter/render/shaders/pp.blur.frag.glsl
    klotter/render/shaders/pp.always.frag.glsl
    klotter/render/shaders/skybox.vert.glsl
    klotter/render/shaders/skybox.frag.glsl
)

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
        textures/skybox-back.jpg
        textures/skybox-left.jpg
        textures/skybox-bottom.jpg
        textures/skybox-right.jpg
        textures/skybox-front.jpg
        textures/skybox-top.jpg
    AS_TEXT
        ${shaders}
)

set(src_render

    klotter/render/rendermode.cc klotter/render/rendermode.h
    klotter/render/cullface.cc klotter/render/cullface.h
    klotter/render/blend.cc klotter/render/blend.h
    klotter/render/compare.cc klotter/render/compare.h
    klotter/render/stencilaction.cc klotter/render/stencilaction.h
    klotter/render/opengl_states.cc klotter/render/opengl_states.h
    klotter/render/render_settings.cc klotter/render/render_settings.h
    klotter/render/material.cc klotter/render/material.h
    klotter/render/world.cc klotter/render/world.h
    klotter/render/debug.cc klotter/render/debug.h
    klotter/render/postproc.cc klotter/render/postproc.h
    klotter/render/shader_resource.cc klotter/render/shader_resource.h
    klotter/render/statechanger.cc klotter/render/statechanger.h
    klotter/render/fullscreen.cc klotter/render/fullscreen.h

    klotter/render/renderer.cc klotter/render/renderer.h
    klotter/render/renderer.pimpl.cc klotter/render/renderer.pimpl.h
    klotter/render/camera.cc klotter/render/camera.h
    klotter/render/opengl_utils.cc klotter/render/opengl_utils.h
    klotter/render/opengl_labels.cc klotter/render/opengl_labels.h

    klotter/render/shader.cc klotter/render/shader.h
    klotter/render/shader.source.cc klotter/render/shader.source.h

    klotter/render/uniform.cc klotter/render/uniform.h
    klotter/render/texture.cc klotter/render/texture.h
    klotter/render/texture.io.h
    klotter/render/assets.cc klotter/render/assets.h

    klotter/render/vertex_layout.cc klotter/render/vertex_layout.h
    klotter/render/uniform_buffer.cc klotter/render/uniform_buffer.h
    klotter/render/linebatch.cc klotter/render/linebatch.h

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
    klotter/cpp.cc klotter/cpp.h

    klotter/bitfield.h
    klotter/feature_flags.h

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
set_target_properties(klotter PROPERTIES FOLDER "Klotter")
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
source_group("pch" FILES ${src_pch})
source_group("_assets" FILES ${src_assets})
source_group("shaders" FILES ${shaders})

cmake_source_group()

add_library(klotter::klotter ALIAS klotter)


set(src_test
    klotter/main.test.cc
    klotter/scurve.test.cc
    klotter/render/texture.test.cc
    klotter/render/vertex_layout.test.cc
    klotter/render/uniform_buffer.test.cc
)
source_group("" FILES ${src_test})
add_executable(test_klotter ${src_test})
set_target_properties(test_klotter PROPERTIES FOLDER "Klotter")
target_link_libraries(test_klotter
    catchy::catchy
    klotter::klotter
    klotter::project_options
)
catch_discover_tests(test_klotter)
