set(TARGET_NAME UnitTestRunner)

add_library(pch_lib "dummy.cpp")
target_precompile_headers(pch_lib PUBLIC pch.hpp)

add_subdirectory(tests)

target_link_libraries(tests PRIVATE pch_lib)
target_precompile_headers(tests REUSE_FROM pch_lib)

add_executable(${TARGET_NAME} Main.cpp )
target_link_libraries(${TARGET_NAME} tests)


add_test(NAME tests COMMAND ${TARGET_NAME})

add_subdirectory(compile_time_tests)