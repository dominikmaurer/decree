cmake_minimum_required(VERSION 3.28)
project(condair_error_handling_library VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    if(NOT CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
        message(FATAL_ERROR "C++20 named modules require the Ninja generator.\nRe-run CMake with: cmake -G Ninja")
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
    message(WARNING "GCC 14+ is recommended for stable C++20 module support (found ${CMAKE_CXX_COMPILER_VERSION})")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17")
    message(WARNING "Clang 17+ is recommended for stable C++20 module support (found ${CMAKE_CXX_COMPILER_VERSION})")
endif()

add_library(condair_error_handling_library)
add_library(condair::error_handling_library ALIAS condair_error_handling_library)

target_sources(condair_error_handling_library
    PUBLIC
        FILE_SET CXX_MODULES
        FILES
            src/error_concept.cppm
            src/error_type.cppm
)

option(CONDAIR_ERROR_HANDLING_BUILD_TESTS "Build unit tests" OFF)

if(CONDAIR_ERROR_HANDLING_BUILD_TESTS AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(tests)
endif()
