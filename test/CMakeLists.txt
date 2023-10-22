set(TARGET_NAME UnitTestRunner)

add_executable(${TARGET_NAME} Main.cpp "test.cpp" )

add_test(NAME tests COMMAND ${TARGET_NAME})

add_subdirectory(compile_time_tests)