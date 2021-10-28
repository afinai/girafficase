#pragma once
#include <core/logger.hpp>

namespace giraffic
{
	namespace udp
	{		
		using packet_callback = std::function<void(const std::string& point, std::int32_t sn)>;

		class server final
		{
			public:
			server(const std::string& addrr, const packet_callback& cb, std::shared_ptr<giraffic::core::logger> logger);
			~server();
			
			private:
				void packet_handler(const std::string& point, std::int32_t sn);

			private:
				packet_callback m_cb;
				std::shared_ptr<giraffic::core::logger> m_logger;
				Poco::Net::UDPHandler::List m_handlers;
				std::unique_ptr<Poco::Net::UDPServer> m_server;
		};
	}
}