cmake_minimum_required(VERSION 3.15.0)
project(flectron VERSION 0.1.0)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/GetDependencies.cmake")

if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
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
  PUBLIC ${GLFW_INCLUDE_DIR}
  PUBLIC ${GLEW_INCLUDE_DIR} 
  PUBLIC ${GLM_INCLUDE_DIR}
  PUBLIC ${STB_IMAGE_INCLUDE_DIR}
  PUBLIC ${OPTICK_INCLUDE_DIR}
  PUBLIC ${ENTT_INCLUDE_DIR}
  PUBLIC ${SPDLOG_INCLUDE_DIR}
)

target_link_libraries(flectron 
  glfw
  libglew_static
  glm
  stb_image
  EnTT
  spdlog::spdlog
)

if(MSVC)
  target_link_libraries(flectron OptickCore)
endif()

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  set(FLECTRON_STANDALONE TRUE)
endif()

option(FLECTRON_BUILD_TESTS "Build flectron tests" ${FLECTRON_STANDALONE})
option(FLECTRON_BUILD_DOCS "Build flectron documentation" ON)
option(FLECTRON_INSTALL "Install the flectron library" ON)

set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_NAME "Flectron")
set(CPACK_PACKAGE_VENDOR "Patryk Janiak")
set(CPACK_PACKAGE_DESCRIPTION "Flectron is a 2D engine written in C++")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/morswin22/flectron")
# set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/flectron.png")

if(FLECTRON_BUILD_TESTS)
  include(InstallRequiredSystemLibraries)
  set(CPACK_PACKAGE_EXECUTABLES "flectron-demo;Flectron Demo" "flectron-terrain;Flectron Terrain Generator" "flectron-piday;Flectron Pi Day")
  # set(CPACK_CREATE_DESKTOP_LINKS "flectron-demo;Flectron Demo" "flectron-terrain;Terrain Generator")
endif()

set(CPACK_THREADS 0)
include(CPack)

cpack_add_component_group(flectron_core DISPLAY_NAME "Flectron" DESCRIPTION "Flectron core library")

if (FLECTRON_INSTALL)
  set_target_properties(flectron PROPERTIES PUBLIC_HEADER "include/flectron.hpp")

  install(DIRECTORY include/ 
    DESTINATION include/
    FILES_MATCHING PATTERN "*.hpp"
    PATTERN "include/flectron.hpp" EXCLUDE
  )
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION include/flectron)
  install(TARGETS flectron 
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/flectron
    COMPONENT flectron_lib
  )
  cpack_add_component(flectron_lib DISPLAY_NAME "Library" DESCRIPTION "Flectron library" GROUP flectron_core)
endif()

if(FLECTRON_BUILD_DOCS)
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(flectron-docs ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generate flectron documentation" 
    )

    if(FLECTRON_INSTALL)
      include(GNUInstallDirs)
      install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/
        DESTINATION ${CMAKE_INSTALL_DOCDIR}
        COMPONENT flectron_docs
      )
      cpack_add_component(flectron_docs DISPLAY_NAME "Documentation" DESCRIPTION "Flectron documentation" GROUP flectron_core)
    endif()
  else()
    message(FATAL_ERROR "Doxygen needs to be installed to generate flectron documentation")
  endif()
endif()

if(FLECTRON_BUILD_TESTS)
  add_subdirectory(tests)
endif()
