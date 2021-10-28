
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../messages")

add_executable(losses_test 
				${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
				${SRC_DIR}/losses/detector.cpp
				${SRC_DIR}/core/error.cpp
				${SRC_DIR}/losses/detector.hpp
				${SRC_DIR}/core/headers.hpp
				${SRC_DIR}/core/error.hpp
				${SRC_DIR}/core/logger.hpp)
				
target_include_directories(losses_test PRIVATE ${SRC_DIR})
target_include_directories(losses_test PRIVATE ${SRC_DIR}/messages)
target_link_libraries(losses_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
target_link_libraries(losses_test PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})