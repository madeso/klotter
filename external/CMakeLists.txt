###################################################################################################
# stb
add_subdirectory(stb)



###################################################################################################
# glad
add_library(external_glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
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
# SDL2
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
add_library(external::sdl2 ALIAS external_sdl2)



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


