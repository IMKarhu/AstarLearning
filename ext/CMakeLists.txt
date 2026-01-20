set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs")
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glad)
add_subdirectory(assimp-5.4.3)

