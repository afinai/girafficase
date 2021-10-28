#include "server.hpp"
#include <messages.hpp>

namespace giraffic::udp
{
	

	struct udp_handler : public Poco::Net::UDPHandler
	{
		udp_handler(const std::string& point, const packet_callback& cb)
			: m_point(point), m_cb(cb)
		{
		}

		void processData(char *buf) override
		{
			auto _payload = payload(buf);

			auto _request = giraffic_messages::Getudp_request(_payload);

			m_cb(m_point, _request->id());

			std::memset(buf, 0, blockSize());
		}

		void processError(char *buf) override
		{
			if (std::string(error(buf)) != "error")
			{

			}

			std::memset(buf, 0, blockSize());
		}

		private:
			std::string m_point;
			packet_callback m_cb;
	};
	
	server::server(const std::string& addrr, const packet_callback& cb, std::shared_ptr<giraffic::core::logger> logger)
	: m_cb(cb), m_logger(logger)
	{
		m_handlers.push_back(new udp_handler(addrr, std::bind(&server::packet_handler, this, std::placeholders::_1, std::placeholders::_2)));
		
		m_server = std::make_unique<Poco::Net::UDPServer>(m_handlers, Poco::Net::SocketAddress(addrr));
	}
	
	server::~server()
	{
		
	}

	void server::packet_handler(const std::string& point, std::int32_t sn)
	{
		m_cb(point, sn);
	}
}