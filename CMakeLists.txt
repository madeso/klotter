cmake_minimum_required(VERSION 3.15)
project(klotter CXX C)


###################################################################################################
# global setup
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
include(cmake/StandardProjectSettings.cmake)

Include(FetchContent)
include(ExternalProject)

###################################################################################################
# functions
function(cmake_source_group)
    source_group("_cmake/pch" REGULAR_EXPRESSION "cmake_pch.[ch]xx$")
    source_group("_cmake/rules" REGULAR_EXPRESSION "\.rule$")
    source_group("_cmake" REGULAR_EXPRESSION "CMakeLists\.txt")
endfunction()

function(target_feature_flags TARGET VIS) # ...targets to enable/disable
    foreach(NAME IN LISTS ARGN)
        if(${${NAME}})
            target_compile_definitions(${TARGET} ${VIS} "-D${NAME}=FF_IS_ENABLED")
        else()
            target_compile_definitions(${TARGET} ${VIS} "-D${NAME}=FF_IS_DISABLED")
        endif()
    endforeach()
endfunction()



###################################################################################################
# options
set(DEFAULT_EU_SDL_SOURCE "FindLibrary")
if(WIN32)
    set(DEFAULT_EU_SDL_SOURCE "FetchContent")
endif()
set(EU_SDL_SOURCE "${DEFAULT_EU_SDL_SOURCE}" CACHE STRING "How to get SDL library (FindLibrary or FetchContent)")
set_property(CACHE EU_SDL_SOURCE PROPERTY STRINGS FindLibrary FetchContent)
if(EU_SDL_SOURCE STREQUAL "FindLibrary")
    set(EU_SDL_SOURCE_FETCH OFF)
    set(EU_SDL_SOURCE_FIND ON)
elseif(EU_SDL_SOURCE STREQUAL "FetchContent")
    set(EU_SDL_SOURCE_FETCH ON)
    set(EU_SDL_SOURCE_FIND OFF)
else()
    message(ERROR "invalid EU_SDL_SOURCE (${EU_SDL_SOURCE})")
endif()

option(BLUR_USE_GAUSS "Use gauss in blur post-proccessing" TRUE)
message(STATUS "BLUR_USE_GAUSS: ${BLUR_USE_GAUSS}")

option(GENERATE_COVERAGE "generate code coverage information" OFF)
message(STATUS "Generating coverage information: ${GENERATE_COVERAGE}")

option(ENABLE_GL_DEBUG "enable opengl debug information" ON)
message(STATUS "Open GL debug information: ${ENABLE_GL_DEBUG}")

option(ENABLE_THEMES "test themes" OFF)
message(STATUS "Test themes: ${ENABLE_THEMES}")


###################################################################################################
# helper libraries
add_library(edit_and_continue INTERFACE)
if(MSVC)
    # ZI = add debug information with edit and continue support to the target
    # Zi = same with edit and continue disabled
    target_compile_options(edit_and_continue INTERFACE "/ZI")
    target_link_options(edit_and_continue INTERFACE "/INCREMENTAL")
endif()
add_library(klotter::edit_and_continue ALIAS edit_and_continue)

add_library(project_options INTERFACE)
target_link_libraries(project_options INTERFACE klotter::edit_and_continue)
target_compile_features(project_options INTERFACE cxx_std_20)
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)
target_feature_flags(project_options INTERFACE
    BLUR_USE_GAUSS
    ENABLE_GL_DEBUG
    ENABLE_THEMES
)

if(GENERATE_COVERAGE)
    # inspired by https://github.com/JoakimSoderberg/coveralls-cmake
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        # supported
    else()
        message(FATAL_ERROR "Compiler ${CMAKE_C_COMPILER_ID} is not supported!")
    endif()

    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "Code coverage results with an optimised (non-Debug) build may be misleading!")
    endif()

    message(STATUS "Coverage enabled")

    # based on
    # https://github.com/cpp-best-practices/cpp_starter_project/blob/main/cmake/Sanitizers.cmake#L6

    # --coverage is a synonym for -fprofile-arcs -ftest-coverage (when compiling) and -lgcov (when linking).
    # https://gcc.gnu.org/onlinedocs/gcc-6.2.0/gcc/Instrumentation-Options.html
    target_compile_options(project_options INTERFACE -fprofile-arcs -ftest-coverage -O0 -g)
    target_link_libraries(project_options INTERFACE -lgcov)
endif()

add_library(klotter::project_options ALIAS project_options)

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)
add_library(klotter::project_warnings ALIAS project_warnings)



###################################################################################################
## CTest integration
include(CTest)
enable_testing()
set_target_properties(Continuous PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Experimental PROPERTIES FOLDER "_cmake/tests")
set_target_properties(Nightly PROPERTIES FOLDER "_cmake/tests")
set_target_properties(NightlyMemoryCheck PROPERTIES FOLDER "_cmake/tests")


###################################################################################################
# start adding sub folders
add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(examples)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "example")
