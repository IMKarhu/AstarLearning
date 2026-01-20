target_sources(Game
    PRIVATE
        camera.h
        cubeVerts.h
        grid.h
        grid.cpp
        shader.h
        shader.cpp
        texture.h
        texture.cpp
        window.cpp
        window.h
)

target_include_directories(Game
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
