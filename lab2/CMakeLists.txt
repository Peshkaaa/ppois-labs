cmake_minimum_required(VERSION 3.16)
project(hotel_lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HOTEL_COVERAGE "Build with gcov coverage instrumentation" OFF)

if(HOTEL_COVERAGE)
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
endif()

if(MSVC)
    set(HOTEL_WARNINGS /W4)
else()
    set(HOTEL_WARNINGS -Wall -Wextra -Wpedantic)
endif()

file(GLOB HOTEL_SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB CLI_SOURCES CONFIGURE_DEPENDS cli/*.cpp)
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)

add_library(hotel_core STATIC ${HOTEL_SOURCES})
target_include_directories(hotel_core PUBLIC include)
target_compile_options(hotel_core PRIVATE ${HOTEL_WARNINGS})

add_library(hotel_cli_lib STATIC ${CLI_SOURCES})
target_include_directories(hotel_cli_lib PUBLIC cli)
target_link_libraries(hotel_cli_lib PUBLIC hotel_core)
target_compile_options(hotel_cli_lib PRIVATE ${HOTEL_WARNINGS})

add_executable(hotel_cli app/main.cpp)
target_link_libraries(hotel_cli PRIVATE hotel_cli_lib)

enable_testing()
add_executable(hotel_tests ${TEST_SOURCES})
target_include_directories(hotel_tests PRIVATE tests)
target_link_libraries(hotel_tests PRIVATE hotel_cli_lib)
add_test(NAME hotel_tests COMMAND hotel_tests)