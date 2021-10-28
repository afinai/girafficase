#pragma once 
#include <core/logger.hpp>

namespace giraffic
{
	class app final : public Poco::Util::ServerApplication
	{		
		using app_config = struct
		{
			std::int32_t _udp_packets_gape;
			std::int32_t _udp_packet_delay;
			std::int32_t _udp_packet_window;
			std::string _udp_srv_addrr;
			std::string _data_folder;
		};
		
	public:
		explicit app();
		~app();

		app(const app&) = delete;
		app& operator = (const app&) = delete;

	private:
		void startup();
		void main();
		void exit();
		int main(const std::vector<std::string>& args) override final;
		void initialize(Poco::Util::Application& self) override;
		void uninitialize() override;
		void defineOptions(Poco::Util::OptionSet& options) override;
		void handleOption(const std::string& name, const std::string& value) override;
				
	private:
		app_config m_config;
		std::shared_ptr<giraffic::core::logger> m_logger;
		std::jthread m_main_thread;
		std::mutex m_lock;
		std::jthread m_traffic_thread;
		std::mutex m_traffic_lock;
	};
}