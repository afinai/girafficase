
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../../messages")

add_executable(primes_test 
				${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
				${SRC_DIR}/primes/detector.cpp
				${SRC_DIR}/core/error.cpp
				${SRC_DIR}/primes/detector.hpp
				${SRC_DIR}/core/headers.hpp
				${SRC_DIR}/core/error.hpp
				${SRC_DIR}/core/logger.hpp)
				
target_include_directories(primes_test PRIVATE ${SRC_DIR})
target_include_directories(primes_test PRIVATE ${SRC_DIR}/messages)
target_link_libraries(primes_test PRIVATE GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
target_link_libraries(primes_test PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})