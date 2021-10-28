cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(giraffic CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

find_package(Flatbuffers REQUIRED)
find_package(Boost REQUIRED)
find_package(Poco REQUIRED Foundation Util Net)
find_package(Threads REQUIRED)
find_package(GTest CONFIG REQUIRED)

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)

add_library(spdlogger INTERFACE)
target_include_directories(spdlogger INTERFACE "$<BUILD_INTERFACE:${CONTRIB_DIR}/spdlog/include>")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DRS})

link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory("src/messages")
add_subdirectory("src/app")
add_subdirectory("src/udp/client")
add_subdirectory("tests/losses")
add_subdirectory("tests/primes")

add_dependencies("giraffic" "messages")
