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

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_NAME "Flectron")
set(CPACK_PACKAGE_VENDOR "Patryk Janiak")
set(CPACK_PACKAGE_DESCRIPTION "Flectron is a 2D engine written in C++")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/morswin22/flectron")
# set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/flectron.png")
set(CPACK_PACKAGE_EXECUTABLES "flectron-demo;Flectron Demo" "flectron-terrain;Terrain Generator")
# set(CPACK_CREATE_DESKTOP_LINKS "flectron-demo;Flectron Demo" "flectron-terrain;Terrain Generator")
set(CPACK_THREADS 0)
include(CPack)

option(FLECTRON_INSTALL "Install the flectron library" ON)
option(FLECTRON_BUILD_DOCS "Build flectron documentation" OFF)
option(FLECTRON_INSTALL_DOCS "Install flectron documentation" OFF)
option(FLECTRON_BUILD_TESTS "Build flectron tests" OFF)

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

if(FLECTRON_BUILD_DOCS OR FLECTRON_INSTALL_DOCS)
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

    if(FLECTRON_INSTALL_DOCS)
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
