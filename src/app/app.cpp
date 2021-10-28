#include "app.hpp"

namespace detail
{
	static const std::string _logger_name("giraffic");

	static const std::string _logger_file("giraffic.log");
	static const std::string _max_prime_number_property("max_prime_number");
	static const std::string _max_prime_portion_property("max_prime_portion");
	static const std::string _max_prime_workers_property("max_prime_workers");
	static const std::string _max_traffic_points_property("max_traffic_points");
	static const std::string _traffic_packet_delay_property("traffic_packet_delay");
	static const std::string _traffic_packets_window_property("traffic_packets_window");
	static const std::string _traffic_packets_slice_property("traffic_packets_slice");
	static const std::string _http_server_property("http_server");
	static const std::string _logger_level_property("logger_level");
	static const std::string _logger_sink_property("logger_sink");
				
	std::shared_ptr<giraffic::http::server> create_http_server(const std::string& addrr, std::shared_ptr<giraffic::core::logger> logger)
	{
		return std::make_shared<giraffic::http::server>(addrr, logger);
	}
		
	std::shared_ptr<giraffic::traffic_detector> create_traffic_detector(const std::string& addrr, std::int32_t packet_delay, std::int32_t packets_window,
		std::int32_t packets_slice, std::shared_ptr<giraffic::core::logger> logger, const giraffic::udp::packet_callback& cb)
	{
		auto _traffic_detector = std::make_shared<giraffic::traffic_detector>();

		_traffic_detector->_udp_server = std::make_unique<giraffic::udp::server>(addrr, cb, logger);
		_traffic_detector->_losses_detector = std::make_unique<giraffic::losses::detector>(std::chrono::seconds(packet_delay), std::chrono::seconds(packets_slice), packets_window);
		
		return _traffic_detector;
	}
	
	std::shared_ptr<giraffic::primes::detector> create_primes_detector(std::uint64_t max_number, std::uint64_t portion, std::size_t workers, std::shared_ptr<giraffic::core::logger> logger)
	{
		return std::make_shared<giraffic::primes::detector>(max_number, portion, workers, logger);
	}
}

namespace giraffic
{
	app::app()
	:m_max_traffic_points(0)
	{
	}
	
	app::~app()
	{
	}
	
	void app::startup()
	{
		m_config._http_srv_addrr = config().getString(detail::_http_server_property, "localhost:10052"),
			
		m_config._max_prime_number = config().getInt64(detail::_max_prime_number_property, 100500);
		m_config._max_prime_portion = config().getUInt64(detail::_max_prime_portion_property, 10000);
		m_config._max_prime_workers = config().getInt(detail::_max_prime_workers_property, 4);

		m_config._max_traffic_points = config().getInt(detail::_max_traffic_points_property, 1);
		m_config._traffic_packet_delay = config().getInt(detail::_traffic_packet_delay_property, 1);
		m_config._traffic_packets_window = config().getInt(detail::_traffic_packets_window_property, 8);
		m_config._traffic_packets_slice = config().getInt(detail::_traffic_packets_slice_property, 2);

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
		
		//Poco::ThreadPool::defaultPool().addCapacity(std::thread::hardware_conccurency);

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
		m_logger->info("app::main => http_srv_addrr : {}", m_config._http_srv_addrr);

		if (!std::filesystem::exists(m_config._data_folder))
		{
			auto _res = std::filesystem::create_directories(m_config._data_folder);
		}

		m_primes_detector = detail::create_primes_detector(m_config._max_prime_number, m_config._max_prime_portion, m_config._max_prime_workers, m_logger);

		m_http_server = detail::create_http_server(m_config._http_srv_addrr, m_logger);
						
		m_http_server->handle("GET", "is_prime", std::bind(&app::http_is_prime_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_http_server->handle("PUT", "add_traffic_point", std::bind(&app::http_add_traffic_point_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_http_server->handle("DELETE", "rem_traffic_point", std::bind(&app::http_rem_traffic_point_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		m_http_server->handle("GET", "traffic_point_losses", std::bind(&app::http_traffic_point_losses_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));	
		
		m_http_server->start();

		m_logger->info("app::main => ok");
	}
		
	giraffic::core::errc app::http_is_prime_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params)
	{
		auto _number = params.get("number", "1");
		
		m_logger->info("app::http_is_prime_callback => check number {} for prime.", _number);

		auto _inumber = std::stoll(_number);

		if(_inumber > m_config._max_prime_number)
		{
			m_logger->info("app::http_is_prime_callback => params number {} more then max {}",_inumber, m_config._max_prime_number);
			
			response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
			response.send();
		
			return giraffic::core::errc::fail;
		}
		
		if(m_primes_detector->is_prime(_inumber))
		{
			m_logger->info("app::http_is_prime_callback => number {} is prime!", _inumber);
			
			response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
			response.send();

			return giraffic::core::errc::success;
		}
		
		m_logger->info("app::http_is_prime_callback => number {} is not prime!", _inumber);
		
		response.setStatus((Poco::Net::HTTPResponse::HTTPStatus)425);
		response.send();
		
		return giraffic::core::errc::success;
	}
	
	giraffic::core::errc app::http_add_traffic_point_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params)
	{
		auto _point = params.get("point", "");

		m_logger->info("app::http_add_traffic_point_callback => try to add _point {}", _point);
		
		if(m_max_traffic_points > m_config._max_traffic_points)
		{
			m_logger->info("app::http_add_traffic_point_callback => execced total {} traffic points count {}", m_max_traffic_points.load(), m_config._max_traffic_points);
			
			response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
			response.send();
		
			return giraffic::core::errc::fail;
		}
		
		{
			std::shared_lock<std::shared_mutex> _lock(m_traffic_detectors_lock);
			if(auto _it = m_traffic_detectors.find(_point); _it != m_traffic_detectors.end())
			{
				m_logger->info("app::http_add_traffic_point_callback => _traffic_point {} already exists!", _point);
		
				response.setStatus((Poco::Net::HTTPResponse::HTTPStatus)425);
				response.send();
				
				return giraffic::core::errc::success;
			}
		}
		
		auto _traffic_point = detail::create_traffic_detector(_point, m_config._traffic_packet_delay, m_config._traffic_packets_window, m_config._traffic_packets_slice, m_logger,
			std::bind(&app::traffic_packet_callback, this, std::placeholders::_1, std::placeholders::_2));	

		if(_traffic_point)
		{
			{
				++m_max_traffic_points;
				
				std::unique_lock<std::shared_mutex> _lock(m_traffic_detectors_lock);
				m_traffic_detectors[_point] = _traffic_point;
			}			
			
			m_logger->info("app::http_add_traffic_point_callback => _traffic_point {} has been added, current {}!", _point, m_max_traffic_points.load());
			
			response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
			response.send();

			return giraffic::core::errc::success;
		}
		
		m_logger->info("app::http_add_traffic_point_callback => _traffic_point {} do not created!", _point);
		
		response.setStatus((Poco::Net::HTTPResponse::HTTPStatus)425);
		response.send();
		
		return giraffic::core::errc::not_implement;
	}
	
	giraffic::core::errc app::http_rem_traffic_point_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params)
	{
		auto _point = params.get("point", "");

		m_logger->info("app::http_rem_traffic_point_callback => try to rem _point {}", _point);
		
		{
			std::unique_lock<std::shared_mutex> _lock(m_traffic_detectors_lock);
			if(auto _it = m_traffic_detectors.find(_point); _it == m_traffic_detectors.end())
			{
				m_logger->info("app::http_rem_traffic_point_callback => _traffic_point {} do not found!", _point);
		
				response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
				response.send();
				
				return giraffic::core::errc::success;
			}
			else
			{								
				m_traffic_detectors.erase(_it);
				
				--m_max_traffic_points;
				
				m_logger->info("app::http_rem_traffic_point_callback => _traffic_point {} removed success, current {}!", _point, m_max_traffic_points.load());
			}
		}
		
		response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
		response.send();

		return giraffic::core::errc::success;
	}
	
	giraffic::core::errc app::http_traffic_point_losses_callback(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response, const giraffic::http::params& params)
	{
		auto _point = params.get("point", "");

		m_logger->info("app::http_traffic_point_losses_callback => try to get losses for _point {}", _point);
		
		{
			std::shared_lock<std::shared_mutex> _lock(m_traffic_detectors_lock);
			if(auto _it = m_traffic_detectors.find(_point); _it == m_traffic_detectors.end())
			{
				m_logger->info("app::http_traffic_point_losses_callback => _traffic_point {} do not found!", _point);
		
				response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
				response.send();
				
				return giraffic::core::errc::success;
			}
			else
			{								
				auto _curr_losses = _it->second->_losses_detector->GetCurrentLoss();
				auto _avg_losses = _it->second->_losses_detector->GetAverageLoss();
				
				m_logger->info("app::http_traffic_point_losses_callback => losses for point {} : current {} / average {}.", _point, _curr_losses, _avg_losses);
				
				boost::property_tree::ptree _ptree;	
				
				_ptree.add("point", _point);
				_ptree.add("losses.average", _avg_losses);
				_ptree.add("losses.current", _curr_losses);
				
				response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
				response.setContentType("application/json");

				std::ostream& stream = response.send();
				boost::property_tree::write_json(stream, _ptree);
			}
		}
		
		response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
		response.send();

		return giraffic::core::errc::success;
	}

	void app::traffic_packet_callback(const std::string& point, std::int32_t sn)
	{
		std::shared_lock<std::shared_mutex> _lock(m_traffic_detectors_lock);
		if (auto _it = m_traffic_detectors.find(point); _it != m_traffic_detectors.end())
		{
			m_logger->debug("app::traffic_packet_callback => point {} : packet sn {}.", point, sn);

			_it->second->_losses_detector->OnPacketReceived(sn);
		}
	}
}