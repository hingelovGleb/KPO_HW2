cmake_minimum_required(VERSION 3.16)
project(POS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(pos_core
    domain/domain.cpp
    application/application.cpp
    infrastructure/infrastructure.cpp
)
target_include_directories(pos_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(pos main.cpp infrastructure/console.cpp)
target_link_libraries(pos PRIVATE pos_core)

add_executable(pos_tests tests/tests.cpp)
target_link_libraries(pos_tests PRIVATE pos_core)
enable_testing()
add_test(NAME domain_tests COMMAND pos_tests)
