project(MindlessEngine)

configure_file(include/MindlessEngine/shaders/basic.vert shaders/basic.vert COPYONLY)
configure_file(include/MindlessEngine/shaders/basic.frag shaders/basic.frag COPYONLY)

set(MindlessEngineSource
  src/random.cpp
  src/vector.cpp
  src/math.cpp
  src/body.cpp
  src/transform.cpp
  src/renderer.cpp
  src/world.cpp
  src/window.cpp
  src/game.cpp
  include/MindlessEngine/random.hpp
  include/MindlessEngine/vector.hpp
  include/MindlessEngine/math.hpp
  include/MindlessEngine/body.hpp
  include/MindlessEngine/transform.hpp
  include/MindlessEngine/renderer.hpp
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
)

target_link_libraries(MindlessEngine 
  glfw
  libglew_static
  glm
)
