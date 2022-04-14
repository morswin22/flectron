set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(FetchContent)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        076bfd55be45e7ba5c887d4b32aa03d26881a1fb
)

FetchContent_Declare(
  glew
  GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
  GIT_TAG 883e35a3d493d93fa27da5abb6225654c360f9d0
)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 2759ceca04db5c36d2806a2280f51b83a6749c12
)

FetchContent_Declare(
  stb_image
  GIT_REPOSITORY https://github.com/morswin22/stb_image.git
  GIT_TAG 526ae60d4319dc35eb9be9bd26c8249918d5a86f
)

FetchContent_Declare(
  optick
  GIT_REPOSITORY https://github.com/bombomby/optick.git
  GIT_TAG 88f498902e9abbd45369bd824ba63a159272b115
)

FetchContent_Declare(
  entt
  GIT_REPOSITORY https://github.com/skypjack/entt.git
  GIT_TAG 0ec763131043647d4c64fb1f6274da12c898b0ca
)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG 76fb40d95455f249bd70824ecfcae7a8f0930fa3
)

FetchContent_MakeAvailable(glfw glew glm stb_image optick entt spdlog)

set(GLFW_INCLUDE_DIR "${glfw_SOURCE_DIR}/include")
set(GLEW_INCLUDE_DIR "${glew_SOURCE_DIR}/include")
set(GLM_INCLUDE_DIR "${glm_SOURCE_DIR}/glm")
set(STB_IMAGE_INCLUDE_DIR "${stb_image_SOURCE_DIR}/")
set(OPTICK_INCLUDE_DIR "${optick_SOURCE_DIR}/src")
set(ENTT_INCLUDE_DIR "${entt_SOURCE_DIR}/single_include")
set(SPDLOG_INCLUDE_DIR "${spdlog_SOURCE_DIR}/include")