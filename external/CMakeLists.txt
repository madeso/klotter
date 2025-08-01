include( ExternalProject )

# todo(Gustav): update from SDL2-2.0.20 to SDL2-2.32.8 (or more modern)
set(SDL_VERSION_STRING "SDL2-2.0.20") # tar.gz name on sdl release page
set(CATCH_GIT_TAG v3.7.1)

###################################################################################################
## catchy
add_subdirectory(catchy)

###################################################################################################
# stb
add_subdirectory(stb)
add_subdirectory(embed)


###################################################################################################
# glad
add_library(external_glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
set_target_properties(external_glad PROPERTIES FOLDER "External")
target_include_directories(external_glad SYSTEM
    PUBLIC
        glad/include
)
target_link_libraries(external_glad
    PUBLIC
        ${CMAKE_DL_LIBS}
)
add_library(external::glad ALIAS external_glad)



###################################################################################################
# Catch2
###################################################################################################
FetchContent_Declare(Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        ${CATCH_GIT_TAG}
)
FetchContent_MakeAvailable(Catch2)
set_target_properties(Catch2 PROPERTIES FOLDER "External")
add_library(catch_main INTERFACE)
target_link_libraries(catch_main INTERFACE Catch2::Catch2 )
add_library(external::catch ALIAS catch_main)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)




###################################################################################################
# SDL2
###################################################################################################
if(EU_SDL_SOURCE_FETCH)
    set(SDL_EXTERN_DIR ${CMAKE_CURRENT_BINARY_DIR}/sdl)
    set(SDL_INSTALL_DIR "${SDL_EXTERN_DIR}/install")
    message(STATUS "Will download and build ${SDL_VERSION_STRING} to ${SDL_EXTERN_DIR} and install to ${SDL_INSTALL_DIR} when building")

    ExternalProject_Add(
        SDL2_external
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://www.libsdl.org/release/${SDL_VERSION_STRING}.zip
        PREFIX "${SDL_EXTERN_DIR}/${SDL_VERSION_STRING}"
        LOG_DIR "${SDL_EXTERN_DIR}/log"
        INSTALL_DIR ${SDL_INSTALL_DIR}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DLIBC=ON
            -DSDL_STATIC=ON
            -DSDL_SHARED=OFF
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        LOG_INSTALL ON
    )
    set_target_properties(SDL2_external PROPERTIES FOLDER "External")

    add_library(external_sdl2 INTERFACE)
    add_dependencies(external_sdl2 SDL2_external)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL_INSTALL_DIR}/include/SDL2
    )
    if(WIN32)
        target_link_libraries(external_sdl2 INTERFACE
            optimized ${SDL_INSTALL_DIR}/lib/SDL2main.lib
            optimized ${SDL_INSTALL_DIR}/lib/SDL2.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2maind.lib
            debug ${SDL_INSTALL_DIR}/lib/SDL2d.lib
            version.lib imm32.lib winmm.lib Setupapi.lib
        )
    else()
        # todo(Gustav): specify debug flag?
        target_link_libraries(external_sdl2 INTERFACE ${SDL_INSTALL_DIR}/lib/libSDL2.a)
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()

if(EU_SDL_SOURCE_FIND)
    find_package(SDL2 REQUIRED)
    add_library(external_sdl2 INTERFACE)
    target_include_directories(external_sdl2 SYSTEM
        INTERFACE
            ${SDL2_INCLUDE_DIR}
    )
    target_link_libraries(external_sdl2
        INTERFACE
            ${SDL2_LIBRARY}
            ${CMAKE_DL_LIBS}
    )
    if(WIN32)
    target_link_libraries(external_sdl2
        INTERFACE
        Setupapi.lib
    )
    endif()
    add_library(external::sdl2 ALIAS external_sdl2)
endif()



###################################################################################################
# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
add_library(external_opengl INTERFACE)
target_link_libraries(external_opengl INTERFACE OpenGL::GL)
add_library(external::opengl ALIAS external_opengl)



###################################################################################################
# glm
add_library(external_glm INTERFACE)
target_include_directories(external_glm SYSTEM
    INTERFACE
        glm
)
add_library(external::glm ALIAS external_glm)




###################################################################################################
# dear_imgui
set(dear_imgui_src
    imgui/imconfig.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h

    imgui/backends/imgui_impl_sdl2.cpp
    imgui/backends/imgui_impl_sdl2.h
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
)
add_library(dear_imgui STATIC ${dear_imgui_src})
set_target_properties(dear_imgui PROPERTIES FOLDER "External")
source_group("" FILES ${dear_imgui_src})
target_include_directories(dear_imgui SYSTEM
    PUBLIC
        imgui/
        imgui/backends/
)
target_link_libraries(dear_imgui
    PUBLIC
        external::glad
        external::opengl
        external::sdl2
)
target_compile_definitions(dear_imgui
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)
add_library(external::imgui ALIAS dear_imgui)



###################################################################################################
# mustache
add_library(mustache INTERFACE)
target_include_directories(mustache SYSTEM
    INTERFACE
        mustache
)
add_library(external::mustache ALIAS mustache)
