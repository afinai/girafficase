#include "app.hpp"
#include <messages.hpp>
#include <Poco/Net/UDPClient.h>

namespace detail
{
	static const std::string _logger_name("giraffic");

	static const std::string _logger_file("udp_client.log");
	static const std::string _logger_level_property("logger_level");
	static const std::string _logger_sink_property("logger_sink");
	static const std::string _udp_server_property("udp_server");
	static const std::string _udp_packet_delay_property("udp_packet_delay");
	static const std::string _udp_packets_window_property("udp_packets_window");
	static const std::string _udp_packets_gape_property("udp_packets_gape");
}

namespace giraffic
{
	app::app()
	{
	}
	
	app::~app()
	{
	}
	
	void app::startup()
	{
		m_config._udp_srv_addrr = config().getString(detail::_udp_server_property, "localhost:10024");
		m_config._udp_packets_gape = config().getInt(detail::_udp_packets_gape_property, 0);
		m_config._udp_packet_delay = config().getInt(detail::_udp_packet_delay_property, 1);
		m_config._udp_packet_window = config().getInt(detail::_udp_packets_window_property, 8);
		m_config._data_folder = config().getString("application.dataDir", "");
				
		std::string _logger_path(m_config._data_folder);
		_logger_path += "\\";
		_logger_path += detail::_logger_file;
				
		auto _logger_level = config().getString(detail::_logger_level_property, "debug");
		auto _logger_sink = config().getString(detail::_logger_sink_property, "console");
		
		m_logger = giraffic::core::create_logger(_logger_level, _logger_sink, detail::_logger_name, _logger_path);
	}

	int app::main(const std::vector<std::string>& /*args*/)
	{
		try
		{	
			startup();
		
			m_main_thread = std::jthread([&] (std::stop_token stoken)
			{
				main();
				
				std::mutex mutex;
				std::unique_lock lock(mutex);
					
				std::condition_variable_any().wait(lock, stoken, [&stoken] { return false; });
					
				if(stoken.stop_requested()) 
				{
					m_logger->info("app::main => exit from main thread by request.");
					return;
				}
			});	
			
			exit();
		}
		catch (const Poco::Exception& ex)
		{
			m_logger->error("app::main => exception [%s]", ex.displayText().c_str());
		}
		catch (const std::exception& ex)
		{
			m_logger->error("app::main => exception! %s", ex.what());
		}
		catch (...)
		{
			m_logger->error("app::main => exception!");
		}

		m_logger->info("app::main => exit.");

		return Application::EXIT_OK;
	}
	
	void app::initialize(Poco::Util::Application& self)
	{
		loadConfiguration();
		ServerApplication::initialize(self);
	}
	
	void app::uninitialize()
	{
		ServerApplication::uninitialize();
	}
	
	void app::defineOptions(Poco::Util::OptionSet& options)
	{
		ServerApplication::defineOptions(options);
	}
	
	void app::handleOption(const std::string& name, const std::string& value)
	{		
		ServerApplication::handleOption(name, value);
	}
	
	void app::exit()
	{
		//todo : add termination logic;
		
		waitForTerminationRequest();
		
		m_logger->info("app::exit => receive stop request.");
		
		m_main_thread.request_stop();
		
		m_logger->info("app::exit => stopping main thread...");
		
		m_main_thread.join();
		
		m_logger->info("app::exit => main thread has been stopped.");
	}
	
	void app::main()
	{			
		m_logger->info("app::main => app_folder : {}", m_config._data_folder);
		m_logger->info("app::main => srv_addrr : {}", m_config._udp_srv_addrr);
		m_logger->info("app::main => packet_delay : {}s", m_config._udp_packet_delay);
		m_logger->info("app::main => packets_window : {}", m_config._udp_packet_window);

		if (!std::filesystem::exists(m_config._data_folder))
		{
			auto _res = std::filesystem::create_directories(m_config._data_folder);
		}

		m_traffic_thread = std::jthread([&] (std::stop_token stoken)
		{
			auto _socket(Poco::Net::SocketAddress(m_config._udp_srv_addrr));

			Poco::Net::UDPClient _client(_socket.host().toString(), _socket.port(), true);

			flatbuffers::FlatBufferBuilder _builder;

			while (!stoken.stop_requested())
			{
				for(auto j = 0; j < m_config._udp_packet_window; ++j)
				{
					auto _igap = m_config._udp_packets_gape ? j % m_config._udp_packets_gape : 0;
					if (_igap == 0)
					{
						auto _request = giraffic_messages::Createudp_request(_builder, j);

						_builder.Finish(_request);

						_client.send(_builder.GetBufferPointer(), _builder.GetSize());
					}
				}
				
				std::this_thread::sleep_for(std::chrono::seconds(m_config._udp_packet_delay));				
			}
		});	


		m_logger->info("app::main => ok");
	}
}