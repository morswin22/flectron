set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FetchContent)

find_package(OpenGL REQUIRED)

find_package(GLFW)
if(NOT GLFW_FOUND)
  set(GLFW_BUILD_DOCS OFF)
  FetchContent_Declare(
    local_glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        076bfd55be45e7ba5c887d4b32aa03d26881a1fb
  )
  FetchContent_MakeAvailable(local_glfw)
  set(GLFW_INCLUDE_DIR "${local_glfw_SOURCE_DIR}/include")
  message(STATUS "GLFW downloaded to ${local_glfw_SOURCE_DIR}")
endif()

find_package(GLEW)
if(NOT GLEW_FOUND)
  FetchContent_Declare(
    local_glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG 883e35a3d493d93fa27da5abb6225654c360f9d0
  )
  FetchContent_MakeAvailable(local_glew)
  set(GLEW_INCLUDE_DIR "${local_glew_SOURCE_DIR}/include")
  message(STATUS "GLEW downloaded to ${local_glew_SOURCE_DIR}")
endif()

find_package(GLM)
if(NOT GLM_FOUND)
  FetchContent_Declare(
    local_glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 2759ceca04db5c36d2806a2280f51b83a6749c12
  )
  FetchContent_MakeAvailable(local_glm)
  set(GLM_INCLUDE_DIR "${local_glm_SOURCE_DIR}/glm")
  message(STATUS "GLM downloaded to ${local_glm_SOURCE_DIR}")
endif()

FetchContent_Declare(
  local_stb_image
  GIT_REPOSITORY https://github.com/morswin22/stb_image.git
  GIT_TAG 526ae60d4319dc35eb9be9bd26c8249918d5a86f
)
FetchContent_MakeAvailable(local_stb_image)
set(STB_IMAGE_INCLUDE_DIR "${local_stb_image_SOURCE_DIR}/stb_image.h")
message(STATUS "STB_IMAGE downloaded to ${local_stb_image_SOURCE_DIR}")

FetchContent_Declare(
  local_optick
  GIT_REPOSITORY https://github.com/bombomby/optick.git
  GIT_TAG 88f498902e9abbd45369bd824ba63a159272b115
)
FetchContent_MakeAvailable(local_optick)
set(OPTICK_INCLUDE_DIR "${local_optick_SOURCE_DIR}/src")
message(STATUS "Optick downloaded to ${local_optick_SOURCE_DIR}")

FetchContent_Declare(
  local_entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG 0ec763131043647d4c64fb1f6274da12c898b0ca
)
FetchContent_MakeAvailable(local_entt)
set(ENTT_INCLUDE_DIR "${local_entt_SOURCE_DIR}/single_include")
message(STATUS "Entt downloaded to ${local_entt_SOURCE_DIR}")