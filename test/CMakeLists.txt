add_subdirectory(tests)

add_executable(UnitTestRunner Main.cpp )
target_link_libraries(UnitTestRunner tests)
target_link_libraries(UnitTestRunner doctest)

add_test(NAME tests COMMAND UnitTestRunner)
