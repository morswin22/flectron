cmake_minimum_required(VERSION 3.0.0)
project(mindless VERSION 0.1.0)

# include(CTest)
# enable_testing()

find_package(OpenGL REQUIRED)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glew)
add_subdirectory(lib/glm)

add_executable(mindless 
  src/main.cpp
  src/common.cpp
)

target_link_libraries(mindless 
  glfw
  libglew_static
  glm
)

target_include_directories(mindless 
  PUBLIC include 
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
