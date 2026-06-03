cmake_minimum_required(VERSION 3.28)
project(decree VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

add_library(decree)
add_library(decree::decree ALIAS decree)

target_compile_features(decree PUBLIC cxx_std_23)
set_target_properties(decree PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(decree PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

option(DECREE_USE_MODULES "Build with C++23 modules" ON)

if(DECREE_USE_MODULES)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(NOT CMAKE_GENERATOR MATCHES "Ninja" AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
            message(FATAL_ERROR "C++23 named modules require the Ninja generator.\nRe-run CMake with: cmake -G Ninja")
        endif()
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
        message(WARNING "GCC 14+ is recommended for stable C++23 module support (found ${CMAKE_CXX_COMPILER_VERSION})")
    endif()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17")
        message(WARNING "Clang 17+ is recommended for stable C++23 module support (found ${CMAKE_CXX_COMPILER_VERSION})")
    endif()

    target_sources(decree
        PUBLIC
            FILE_SET CXX_MODULES
            FILES
                src/error_concept_module.cppm
                src/error_handling_module.cppm
    )
endif()

option(DECREE_BUILD_TESTS "Build unit tests" OFF)

if(DECREE_BUILD_TESTS AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CMakePackageConfigHelpers)

    if(DECREE_USE_MODULES)
        install(TARGETS decree
            EXPORT   decree-targets
            ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
            FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/decree
        )
    else()
        install(TARGETS decree
            EXPORT  decree-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
    endif()

    install(FILES
        src/decree_impl.inc
        src/error_concept_impl.inc
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/decree
    )

    if(NOT DECREE_USE_MODULES)
        install(FILES
            src/error_handling.hpp
            src/error_concept.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/decree
        )
    endif()

    install(EXPORT decree-targets
        FILE        decree-targets.cmake
        NAMESPACE   decree::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/decree
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/decree-config-version.cmake"
        VERSION     ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/decree-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/decree-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/decree
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/decree-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/decree-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/decree
    )
endif()
