
# https://cmake.org/pipermail/cmake/2017-September/066221.html

add_library(DUMMY_COMPILE_TIME_TESTS STATIC EXCLUDE_FROM_ALL 
  "transform_does_not_compile_with_invalid_input_parameter.cpp"
  "transform_does_not_compile_with_invalid_output_parameter.cpp"
)

function(create_compile_time_test NAME)
    set(LIB_NAME "${NAME}_lib")
    set(TEST_NAME "${NAME}_test")
    set(FILE_NAME "${NAME}.cpp")

    add_library(${LIB_NAME} STATIC EXCLUDE_FROM_ALL ${FILE_NAME})
    target_include_directories(${LIB_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/..")

    add_test(NAME ${TEST_NAME}
      COMMAND ${CMAKE_COMMAND} --build . --target ${LIB_NAME} --config $<CONFIGURATION>
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    set_tests_properties(${TEST_NAME} PROPERTIES
      PASS_REGULAR_EXPRESSION "the associated constraints are not satisfied"
    )
endfunction()

file(GLOB tests LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

list(TRANSFORM tests REPLACE ".cpp" "")

foreach(NAME ${tests})
  create_compile_time_test(${NAME})
endforeach()
