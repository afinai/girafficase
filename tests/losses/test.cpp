#include <gtest/gtest.h>
#include <core/headers.hpp>
#include <core/logger.hpp>
#include <losses/detector.hpp>

class LossesTestEnvironment : public::testing::Environment
{
public:
	LossesTestEnvironment(std::int32_t packet_delay, std::int32_t packets_window, std::int32_t packets_slice)
	:_packet_delay(packet_delay), _packets_window(packets_window), _packets_slice(packets_slice)
	{
	}
	
	void SetUp() override
	{
		const std::string pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
		const std::string level("info");

		_logger = spdlog::stdout_color_mt("console");

		_logger->set_level(giraffic::core::detail::get_logger_level(level));
		_logger->set_pattern(pattern);

		_losses_detector = std::make_unique<giraffic::losses::detector>(std::chrono::seconds(_packet_delay), std::chrono::seconds(_packets_slice), _packets_window);
	}

	giraffic::losses::detector* losses_detector() const
	{
		return _losses_detector.get();
	}

	std::int32_t packet_delay() const
	{
		return _packet_delay;
	}

	std::int32_t packets_window() const
	{
		return _packets_window;
	}

	std::int32_t packets_slice() const
	{
		return _packets_slice;
	}

	
protected:
	std::shared_ptr<giraffic::core::logger> _logger;
	std::unique_ptr<giraffic::losses::detector> _losses_detector;
	std::int32_t _packet_delay;
	std::int32_t _packets_window;
	std::int32_t _packets_slice;
};

LossesTestEnvironment* g_LossesTestEnvironment = nullptr;

class LossesTest : public ::testing::Test
{
protected:
	void SetUp()
	{
		_losses_detector = g_LossesTestEnvironment->losses_detector();
		_packet_delay = g_LossesTestEnvironment->packet_delay();
		_packets_window = g_LossesTestEnvironment->packets_window();
		_packets_slice = g_LossesTestEnvironment->packets_slice();
	}

	void TearDown()
	{
		_losses_detector = nullptr;
	}

	giraffic::losses::detector* _losses_detector;
	std::int32_t _packet_delay;
	std::int32_t _packets_window;
	std::int32_t _packets_slice;
};

TEST_F( LossesTest, AddTest )
{
	std::int32_t _total = _packets_window * 64;
	std::int32_t _sleep = _packet_delay;
	std::int32_t _gap = 2;

	for (auto i = 0; i < _total; ++i)
	{
		for(auto j = i; j < _packets_window + i; ++j)
		{
			auto _igap = j % _gap;
			if (_igap == 0)
			{
				_losses_detector->OnPacketReceived(j);
			}
		}
				
		std::this_thread::sleep_for(std::chrono::seconds(_sleep));

		auto _losses= _losses_detector->GetAverageLoss();

		EXPECT_TRUE(_losses_detector->GetAverageLoss() == 0);
	}

	EXPECT_TRUE(_losses_detector->GetCurrentLoss() == 0);
	EXPECT_TRUE(_losses_detector->GetAverageLoss() == 0);
}

int main( int argc, char* argv[] )
{
	std::int32_t _packet_delay = 1;
	std::int32_t _packets_window = 8;
	std::int32_t _packets_slice = 2;

	::testing::InitGoogleTest( &argc, argv );

	g_LossesTestEnvironment = dynamic_cast<LossesTestEnvironment*>(::testing::AddGlobalTestEnvironment(new LossesTestEnvironment(_packet_delay, _packets_window, _packets_slice)));

	return RUN_ALL_TESTS();
}

