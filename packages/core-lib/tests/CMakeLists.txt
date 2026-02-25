cmake_minimum_required(VERSION 3.10)

enable_testing()

add_executable(compilador_test
    ${CMAKE_CURRENT_SOURCE_DIR}/compilador_test.cpp
)
target_link_libraries(compilador_test
    GTest::gtest_main
    compilador
)

include(GoogleTest)
gtest_discover_tests(compilador_test)