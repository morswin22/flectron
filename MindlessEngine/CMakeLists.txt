project(MindlessEngine)

set(MindlessEngineSource
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
  src/collisions.cpp
  src/renderer.cpp
  src/scene.cpp
  src/world.cpp
  src/window.cpp
  src/game.cpp
  include/MindlessEngine/memory.hpp
  include/MindlessEngine/stopwatch.hpp
  include/MindlessEngine/random.hpp
  include/MindlessEngine/color.hpp
  include/MindlessEngine/input.hpp
  include/MindlessEngine/vector.hpp
  include/MindlessEngine/math.hpp
  include/MindlessEngine/aabb.hpp
  include/MindlessEngine/body.hpp
  include/MindlessEngine/transform.hpp
  include/MindlessEngine/collisions.hpp
  include/MindlessEngine/renderer.hpp
  include/MindlessEngine/scene.hpp
  include/MindlessEngine/world.hpp
  include/MindlessEngine/window.hpp
  include/MindlessEngine/game.hpp
  include/MindlessEngine/MindlessEngine.hpp
)

add_library(MindlessEngine STATIC ${MindlessEngineSource})

target_include_directories(MindlessEngine 
  PUBLIC include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw/include
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glew/include  
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/stb_image
)

target_link_libraries(MindlessEngine 
  glfw
  libglew_static
  glm
  stb_image
)
