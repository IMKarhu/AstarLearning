cmake_minimum_required(VERSION 3.28...4.1)

project(aStarLearning)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MINGW OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "Disable Werror for Assimp" FORCE)
    add_compile_options(-Wno-error -Wno-unknown-pragmas)
endif()

include_directories("ext/glad")
include_directories("ext/stb-master")

add_executable(Game
    main.cpp
)

add_subdirectory(ext)
add_subdirectory(src)

target_link_libraries(Game
    PRIVATE glfw
    PUBLIC glad
    PUBLIC glm
    PUBLIC assimp
)

add_custom_command(TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets"
            "${PROJECT_BINARY_DIR}/assets")

