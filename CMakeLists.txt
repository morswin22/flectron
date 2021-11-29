cmake_minimum_required(VERSION 3.0.0)
project(MindlessProjects)

# include(CTest)
# enable_testing()

find_package(OpenGL REQUIRED)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glew)
add_subdirectory(lib/glm)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/optick)

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_USE_MATH_DEFINES")
endif()

project(flectron)

set(flectronSource
  src/memory.cpp
  src/stopwatch.cpp
  src/random.cpp
  src/color.cpp
  src/input.cpp
  src/vector.cpp
  src/math.cpp
  src/aabb.cpp
  src/body.cpp
  src/transform.cpp
  src/grid.cpp
  src/collisions.cpp
  src/renderer.cpp
  src/wfc.cpp
  src/scene.cpp
  src/world.cpp
  src/window.cpp
  src/game.cpp
)

add_library(flectron STATIC ${flectronSource})

target_include_directories(flectron 
  PUBLIC include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include  
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb_image
)

target_link_libraries(flectron 
  glfw
  libglew_static
  glm
  stb_image
)

if(MSVC)
  target_link_libraries(flectron OptickCore)
endif()

add_subdirectory(tests)
