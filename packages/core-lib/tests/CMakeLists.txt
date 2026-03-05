cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_executable(compilador_test
    ${CMAKE_CURRENT_SOURCE_DIR}/compilador_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lalg.cpp
)
target_link_libraries(compilador_test
    GTest::gtest_main
    compilador
)

include(GoogleTest)
gtest_discover_tests(compilador_test)
