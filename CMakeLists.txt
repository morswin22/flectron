cmake_minimum_required(VERSION 3.0.0)
project(flectron VERSION 0.1.0)

find_package(OpenGL REQUIRED)
add_subdirectory(lib/glfw)
add_subdirectory(lib/glew)
add_subdirectory(lib/glm)
add_subdirectory(lib/stb_image)
add_subdirectory(lib/optick)
add_subdirectory(lib/entt)

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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE FLECTRON_SRC_FILES ./src/*.cpp)

add_library(flectron STATIC ${FLECTRON_SRC_FILES})

target_include_directories(flectron 
  PUBLIC include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include  
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb_image
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/entt/single_include
)

target_link_libraries(flectron 
  glfw
  libglew_static
  glm
  stb_image
  EnTT
)

if(MSVC)
  target_link_libraries(flectron OptickCore)
endif()

add_subdirectory(tests)
