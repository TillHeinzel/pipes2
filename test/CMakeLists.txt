set(TARGET_NAME UnitTestRunner)

add_executable(${TARGET_NAME} Main.cpp "test.cpp")

add_test(NAME tests COMMAND ${TARGET_NAME})