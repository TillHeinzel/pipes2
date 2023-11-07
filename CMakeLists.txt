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

add_subdirectory("3pp")
add_subdirectory("pch")
add_subdirectory("src")
add_subdirectory("test")


set(rootDir "${CMAKE_CURRENT_SOURCE_DIR}/pipes")
set(initialFile "pipes.hpp")
set(targetFileName "${CMAKE_CURRENT_BINARY_DIR}/single_header.hpp")
set(desiredOrder detail/Utility detail/GenericImplementation detail)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/single_header.hpp COMMAND 
${CMAKE_COMMAND} 
-DrootDir=${rootDir} 
-DinitialFile=${initialFIle} 
-DtargetFileName=${targetFileName}
-DdesiredOrder=${targetFileName}
-P ${CMAKE_CURRENT_SOURCE_DIR}/makeSingleHeader.cmake 
DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/makeSingleHeader.cmake
)

