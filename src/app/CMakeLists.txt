
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
				${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/app.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/app.hpp
				${APP_HDRS}
				${APP_SRCS})
				
target_include_directories(giraffic PRIVATE ${SRC_DIR})
target_include_directories(giraffic PRIVATE ${SRC_DIR}/messages)
target_link_libraries(giraffic PRIVATE Poco::Util Poco::Net spdlogger flatbuffers ${CMAKE_THREAD_LIBS_INIT})