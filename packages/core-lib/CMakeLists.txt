cmake_minimum_required(VERSION 3.10)
project(compilador)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(compilador STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/analisador_sintatico.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compilador.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compilador_lalg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
)

add_library(examples INTERFACE)
target_compile_definitions(examples
    INTERFACE EXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/examples"
)

target_include_directories(compilador
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)


target_link_libraries(main
    compilador
    examples
)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/googletest-1.17.0)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
