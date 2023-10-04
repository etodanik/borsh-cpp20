# Based on code by Kris Jusiak (kris at jusiak dot net)
#
# Copyright (c) 2019-2020 Kris Jusiak (kris at jusiak dot net)
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
cmake_minimum_required(VERSION 3.21...3.27)
project(
        borsh
        VERSION 1.0.0
        LANGUAGES CXX
)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD
            20
            CACHE STRING "Default value for CXX_STANDARD property of targets."
    )
    option(CMAKE_CXX_STANDARD_REQUIRED "Default value for CXX_STANDARD_REQUIRED property of targets." YES)
    option(CMAKE_CXX_EXTENSIONS "Default value for CXX_EXTENSIONS property of targets." NO)
endif()

option(BORSH_ENABLE_COVERAGE "Run coverage" OFF)
option(BORSH_ENABLE_SANITIZERS "Run static analysis" OFF)
option(BORSH_BUILD_BENCHMARKS "Build the benchmarks" OFF)
option(BORSH_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
option(BORSH_BUILD_TESTS "Build the tests" ${PROJECT_IS_TOP_LEVEL})
option(BORSH_USE_WARNINGS_AS_ERORS "Build the tests" ${PROJECT_IS_TOP_LEVEL})
option(BORSH_ALLOW_CPM_USE "Do not reach out across network for CPM" ON)

if(NOT CMAKE_SKIP_INSTALL_RULES)
    if(BORSH_ALLOW_CPM_USE)
        # ---- Add dependencies via CPM ----
        # see https://github.com/cpm-cmake/CPM.cmake for more info

        include(cmake/CPM.cmake)

        # PackageProject.cmake will be used to make our target installable
        CPMAddPackage(
                NAME PackageProject.cmake
                GITHUB_REPOSITORY TheLartians/PackageProject.cmake
                VERSION 1.10.0
        )
    else()
        include(cmake/PackageProject.cmake)
    endif()
endif()

add_library(borsh INTERFACE)
target_include_directories(borsh INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>)
target_compile_features(borsh INTERFACE cxx_std_20)

if(BORSH_USE_WARNINGS_AS_ERORS)
    include(cmake/WarningsAsErrors.cmake)
endif()

if(BORSH_ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
endif()

if(BORSH_ENABLE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -fno-omit-frame-pointer -fsanitize=address -fsanitize=leak -fsanitize=undefined")
endif()

if(NOT CMAKE_SKIP_INSTALL_RULES)
    packageProject(
            NAME ${PROJECT_NAME}
            VERSION ${PROJECT_VERSION}
            BINARY_DIR ${PROJECT_BINARY_DIR}
            INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
            INCLUDE_DESTINATION ${INCLUDE_INSTALL_DIR}
            # XXX variant: DISABLE_VERSION_SUFFIX YES
            COMPATIBILITY SameMajorVersion
            # Note: only if needed i.e. DEPENDENCIES "fmt 7.1.3; span"
    )
endif()

if(BORSH_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

if(BORSH_BUILD_TESTS)
    enable_testing()

    add_subdirectory(test)
endif()