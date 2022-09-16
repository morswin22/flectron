include(CMakeParseArguments)

function(enable_coverage)
  find_program(GCOV_PATH gcov)
  if (NOT GCOV_PATH)
    message(FATAL_ERROR "gcov not found! Aborting...")
  endif()
  
  find_program(GCOVR_PATH NAMES gcovr)
  if (NOT GCOVR_PATH)
  message(FATAL_ERROR "gcovr not found! Aborting...")
  endif()
  
  find_program(GENHTML_PATH NAMES genhtml genhtml.perl genhtml.bat)

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
endfunction()

function(enable_coverage_for target)
  target_link_libraries(${target} gcov)
endfunction()

function(add_coverage)
  set(options HTML XML)
  set(oneValueArgs NAME EXECUTABLE)
  set(multiValueArgs SOURCES EXCLUDES EXECUTABLE_ARGS DEPENDENCIES)
  cmake_parse_arguments(COVERAGE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  set(sources "")
  foreach(source ${COVERAGE_SOURCES})
    get_filename_component(source ${source} ABSOLUTE)
    list(APPEND sources "-f ${source}")
  endforeach()
  list(REMOVE_DUPLICATES sources)
  string(JOIN " " sources ${sources})

  set(excludes "")
  foreach(excludes ${COVERAGE_EXCLUDES})
    get_filename_component(exclude ${exclude} ABSOLUTE)
    list(APPEND excludes "-e ${exclude}")
  endforeach()
  list(REMOVE_DUPLICATES excludes)
  string(JOIN " " excludes ${excludes})

  if (COVERAGE_HTML)
    if (NOT GENHTML_PATH)
      message(FATAL_ERROR "genhtml not found! Aborting...")
    endif()

    add_custom_target(${COVERAGE_NAME}-html
      COMMAND ${COVERAGE_EXECUTABLE} ${COVERAGE_EXECUTABLE_ARGS}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${COVERAGE_NAME}-html
      COMMAND ${GCOVR_PATH} --html ${COVERAGE_NAME}-html/index.html --html-details ${sources} ${excludes} --object-directory=${CMAKE_CURRENT_BINARY_DIR}

      BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${COVERAGE_NAME}-html/index.html
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS ${COVERAGE_DEPENDENCIES}
      VERBATIM
      COMMENT "Running gcovr to produce HTML code coverage report."
    )

    add_custom_command(TARGET ${COVERAGE_NAME}-html POST_BUILD
      COMMAND ;
      COMMENT "Open ./${COVERAGE_NAME}-html/index.html in your browser to view the coverage report."
    )
  endif()

  if (COVERAGE_XML)
  add_custom_target(${COVERAGE_NAME}-xml
    COMMAND ${COVERAGE_EXECUTABLE} ${COVERAGE_EXECUTABLE_ARGS}
    COMMAND ${GCOVR_PATH} --xml ${COVERAGE_NAME}.xml ${sources} ${excludes} --object-directory=${CMAKE_CURRENT_BINARY_DIR}

    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/${COVERAGE_NAME}.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${COVERAGE_DEPENDENCIES}
    VERBATIM
    COMMENT "Running gcovr to produce XML code coverage report."
  )

  add_custom_command(TARGET ${COVERAGE_NAME}-html POST_BUILD
    COMMAND ;
    COMMENT "Upload ./${COVERAGE_NAME}.xml to codecov.io to view the coverage report."
  )
  endif()
endfunction()