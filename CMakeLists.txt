cmake_minimum_required (VERSION 3.26)

project (pipes VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options("/permissive-")
    add_compile_options("/Zc:preprocessor")
endif()

enable_testing()

add_subdirectory("test")