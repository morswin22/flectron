project(MindlessEngine)

set(MindlessEngineSource
  src/random.cpp
  src/vector.cpp
  src/math.cpp
  src/body.cpp
  src/transform.cpp
  src/world.cpp
  src/window.cpp
  include/MindlessEngine/random.hpp
  include/MindlessEngine/vector.hpp
  include/MindlessEngine/math.hpp
  include/MindlessEngine/body.hpp
  include/MindlessEngine/transform.hpp
  include/MindlessEngine/world.hpp
  include/MindlessEngine/window.hpp
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
