#pragma once 
#include <core/logger.hpp>
#include <http/server/server.hpp>
#include <primes/detector.hpp>
#include <losses/detector.hpp>
#include <udp/server/server.hpp>

namespace giraffic
{
	using traffic_detector = struct
	{
		std::unique_ptr<giraffic::losses::detector> _losses_detector;
		std::unique_ptr<giraffic::udp::server> _udp_server;
	};

	class app final : public Poco::Util::ServerApplication
	{		
		using app_config = struct
		{
			std::string _http_srv_addrr;
			std::string _data_folder;
			std::int64_t _max_prime_number;
			std::uint64_t _max_prime_portion;
			std::size_t _max_prime_workers;
			std::int32_t _max_traffic_points;
			std::int32_t _traffic_packet_delay;
			std::int32_t _traffic_packets_window;
			std::int32_t _traffic_packets_slice;
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
		int  main(const std::vector<std::string>& args) override final;
		void initialize(Poco::Util::Application& self) override final;
		void uninitialize() override final;
		void defineOptions(Poco::Util::OptionSet& options) override final;
		void handleOption(const std::string& name, const std::string& value) override final;
				
	private:
	void traffic_packet_callback(const std::string& point, std::int32_t sn);
	giraffic::core::errc http_is_prime_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params);
	giraffic::core::errc http_add_traffic_point_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params);
	giraffic::core::errc http_rem_traffic_point_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params);
	giraffic::core::errc http_traffic_point_losses_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params);

	private:
		app_config m_config;
		std::shared_ptr<giraffic::core::logger> m_logger;
		std::shared_ptr<giraffic::http::server> m_http_server;
		std::shared_mutex m_traffic_detectors_lock;
		std::map<std::string, std::shared_ptr<traffic_detector>> m_traffic_detectors;
		std::atomic<std::int16_t> m_max_traffic_points;
		std::shared_ptr<giraffic::primes::detector> m_primes_detector;
		std::jthread m_main_thread;
		std::mutex m_lock;
	};
}