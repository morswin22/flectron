cmake_minimum_required(VERSION 3.0.0)
project(MindlessProjects)

# include(CTest)
# enable_testing()

find_package(OpenGL REQUIRED)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glew)
add_subdirectory(lib/glm)

add_subdirectory(MindlessEngine)
add_subdirectory(Mindless)
