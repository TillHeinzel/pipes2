set(TARGET_NAME UnitTestRunner)

add_executable(${TARGET_NAME} Main.cpp "test.cpp" )

add_test(NAME tests COMMAND ${TARGET_NAME})

function(create_compile_time_test NAME)
    
    set(LIB_NAME "${NAME}_lib")
    set(TEST_NAME "${NAME}_test")
    set(FILE_NAME "${NAME}.cpp")

    add_library(${LIB_NAME} STATIC EXCLUDE_FROM_ALL ${FILE_NAME})

    add_test(NAME ${TEST_NAME}
      COMMAND ${CMAKE_COMMAND} --build . --target ${LIB_NAME} --config $<CONFIGURATION>
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    set_tests_properties(${TEST_NAME} PROPERTIES WILL_FAIL TRUE)
endfunction()

create_compile_time_test(transform_does_not_compile_with_invalid_input_parameter)
create_compile_time_test(transform_does_not_compile_with_invalid_output_parameter)