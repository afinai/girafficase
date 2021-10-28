
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../messages")

add_executable(client 
				${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/app.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/app.hpp
				${SRC_DIR}/core/error.cpp
				${SRC_DIR}/core/headers.hpp
				${SRC_DIR}/core/error.hpp
				${SRC_DIR}/core/logger.hpp)
				
target_include_directories(client PRIVATE ${SRC_DIR})
target_include_directories(client PRIVATE ${SRC_DIR}/messages)
target_link_libraries(client PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})