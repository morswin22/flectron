project(flectron-tests)

set(main_runner tests.cpp)
add_executable(flectron-tests ${main_runner})
set_property(TARGET flectron-tests PROPERTY CXX_STANDARD 17)

if(NOT MSVC)
  find_package(Threads REQUIRED)
  target_link_libraries(flectron-tests Threads::Threads)
endif()

file(GLOB TEST_SOURCES *.cpp)
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*${main_runner}")

list(LENGTH TEST_SOURCES TEST_SOURCES_COUNT)
set(TEST_SOURCES_OUTPUTS "")

option(FLECTRON_HEADLESS_TESTS "Run tests in headless mode" OFF)

foreach(test_source ${TEST_SOURCES})
  get_filename_component(test_name ${test_source} NAME_WE)
  set(test_name "flectron-tests-${test_name}")
  add_executable(${test_name} ${test_source})
  target_link_libraries(${test_name} flectron)
  add_dependencies(flectron-tests ${test_name})
  # add_test(${test_name} ${test_name})
  if (MSVC)
    list(APPEND TEST_SOURCES_OUTPUTS "\"${CMAKE_CURRENT_BINARY_DIR}/Debug/${test_name}.exe\"")
  else()
    if(FLECTRON_HEADLESS_TESTS)
      list(APPEND TEST_SOURCES_OUTPUTS "\"xvfb-run ${CMAKE_CURRENT_BINARY_DIR}/${test_name}\"")
    else()
      list(APPEND TEST_SOURCES_OUTPUTS "\"${CMAKE_CURRENT_BINARY_DIR}/${test_name}\"")
    endif()
  endif()
endforeach()

string(JOIN ",\n" TEST_SOURCES_OUTPUTS "${TEST_SOURCES_OUTPUTS}")
set(tests_list_hpp 
"#pragma once
#include <array>
#include <string_view>

constexpr std::array<std::string_view, ${TEST_SOURCES_COUNT}> tests_list = {
  ${TEST_SOURCES_OUTPUTS}
}\;
")

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/tests_list/tests_list.hpp ${tests_list_hpp})
target_include_directories(flectron-tests PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/tests_list)
