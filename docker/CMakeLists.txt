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

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(CONTRIB_DIR ${CMAKE_SOURCE_DIR}/contrib)
set(MESSAGES_DIR ${CMAKE_SOURCE_DIR}/src/messages)

add_library(spdlogger INTERFACE)
target_include_directories(spdlogger INTERFACE "$<BUILD_INTERFACE:${CONTRIB_DIR}/spdlog/include>")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${Boost_INCLUDE_DRS})

link_directories(${Boost_LIBRARY_DIRS})

#add_dependencies("giraffic" "messages")

if (Flatbuffers_FOUND)
    find_program(CMAKE_FLATBUFFERS_COMPILER NAMES flatc)
else()
    message(FATAL_ERROR "FLATBUFFERSR not foud!")
endif()

message(STATUS ${CMAKE_FLATBUFFERS_COMPILER})

file(GLOB schema_files "${MESSAGES_DIR}/schema/*.fbs")

set(schema_header "schema.hpp")
set(messages_header "messages.hpp")

set(input_paths)
foreach(schema_filename ${schema_files})
    list(APPEND input_paths ${schema_filename})
endforeach()

set(output_paths)
foreach(schema_filename ${schema_files})
    get_filename_component(filename "${schema_files}" NAME_WE)
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/generated/${filename}.hpp")
    list(APPEND output_paths ${output_file})
endforeach()

# Copy main header
configure_file(
    "${MESSAGES_DIR}/${schema_header}"
    "${CMAKE_CURRENT_BINARY_DIR}/${messages_header}"
    COPYONLY)
# Append to list of outputs
list(APPEND ${output_paths} "${CMAKE_CURRENT_BINARY_DIR}/${messages_header}")

# Generate C++ headers
add_custom_command(
    OUTPUT ${output_paths}
    COMMAND ${CMAKE_FLATBUFFERS_COMPILER} -o "${CMAKE_CURRENT_BINARY_DIR}/generated" --cpp
    ARGS ${input_paths}
    COMMENT "Generating flattbuffers message C++ headers")

add_custom_target(messages DEPENDS ${output_paths})

include_directories("${CMAKE_CURRENT_BINARY_DIR}/../messages")

set(APP_SRCS
			${SRC_DIR}/core/error.cpp
			${SRC_DIR}/http/server/server.cpp
			${SRC_DIR}/http/server/handler.cpp
			${SRC_DIR}/http/server/dispatcher.cpp
			${SRC_DIR}/http/server/factory.cpp
			${SRC_DIR}/http/utils/params.cpp
			${SRC_DIR}/http/utils/uri.cpp
			${SRC_DIR}/udp/server/server.cpp
			${SRC_DIR}/primes/detector.cpp
			${SRC_DIR}/losses/detector.cpp)
				
set(APP_HDRS
			${SRC_DIR}/core/headers.hpp
			${SRC_DIR}/core/error.hpp
			${SRC_DIR}/core/logger.hpp
			${SRC_DIR}/http/server/server.hpp
			${SRC_DIR}/http/server/handler.hpp
			${SRC_DIR}/http/server/dispatcher.hpp
			${SRC_DIR}/http/server/factory.hpp
			${SRC_DIR}/http/utils/params.hpp
			${SRC_DIR}/http/utils/uri.hpp
			${SRC_DIR}/http/utils/executor.hpp
			${SRC_DIR}/udp/server/server.hpp
			${SRC_DIR}/primes/detector.hpp
			${SRC_DIR}/losses/detector.hpp)
				
add_executable(giraffic
				${SRC_DIR}/app/main.cpp
				${SRC_DIR}/app/app.cpp
				${SRC_DIR}/app/app.hpp
				${APP_HDRS}
				${APP_SRCS})
				
target_include_directories(giraffic PRIVATE ${SRC_DIR})
target_include_directories(giraffic PRIVATE ${SRC_DIR}/messages)
target_link_libraries(giraffic PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})

include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../messages")

add_executable(client 
				${SRC_DIR}/udp/client/main.cpp
				${SRC_DIR}/udp/client/app.cpp
				${SRC_DIR}/udp/client/app.hpp
				${SRC_DIR}/core/error.cpp
				${SRC_DIR}/core/headers.hpp
				${SRC_DIR}/core/error.hpp
				${SRC_DIR}/core/logger.hpp)
				
target_include_directories(client PRIVATE ${SRC_DIR})
target_include_directories(client PRIVATE ${SRC_DIR}/messages)
target_link_libraries(client PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})