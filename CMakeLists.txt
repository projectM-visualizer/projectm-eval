cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(projectM_EvalLib
        VERSION 1.0.0
        LANGUAGES C CXX
        )

include(GNUInstallDirs)

find_package(BISON 3.8)
find_package(FLEX 2.6)

add_subdirectory(projectm-eel)
add_subdirectory(SimpleCompiler)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

