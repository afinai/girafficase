#include <gtest/gtest.h>
#include <primes/detector.hpp>

class PrimesTestEnvironment : public::testing::Environment
{
public:
	PrimesTestEnvironment(std::uint64_t max_number, std::uint64_t portion, std::size_t workers)
	:_max_number(max_number), _portion(portion), _workers(workers)
	{
	}
	
	void SetUp() override
	{
		const std::string pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
		const std::string level("info");

		_logger = spdlog::stdout_color_mt("console");

		_logger->set_level(giraffic::core::detail::get_logger_level(level));
		_logger->set_pattern(pattern);

		_primes_detector = std::make_unique<giraffic::primes::detector>(_max_number,_portion, _workers, _logger);
	}

	giraffic::primes::detector* primes_detector() const
	{
		return _primes_detector.get();
	}

	
protected:
	std::shared_ptr<giraffic::core::logger> _logger;
	std::unique_ptr<giraffic::primes::detector> _primes_detector;
	std::uint64_t _max_number;
	std::uint64_t _portion;
	std::size_t _workers;
};

PrimesTestEnvironment* g_PrimesTestEnvironment = nullptr;

class PrimesTest : public ::testing::Test
{
protected:
	void SetUp()
	{
		_primes_detector = g_PrimesTestEnvironment->primes_detector();
	}

	void TearDown()
	{
		_primes_detector = nullptr;
	}

	giraffic::primes::detector* _primes_detector;
};

TEST_F( PrimesTest, AddTest )
{
	auto _now = std::chrono::system_clock::now();

	_primes_detector->build();

	auto _end = std::chrono::system_clock::now();

	EXPECT_FALSE(_end - _now > std::chrono::seconds(10));

	EXPECT_TRUE(_primes_detector->is_prime(13));
	EXPECT_FALSE(_primes_detector->is_prime(500));
}

int main( int argc, char* argv[] )
{
	std::uint64_t _max_number = 10005000;
	std::uint64_t _portion = 10000;
	std::size_t _workers = 4;
	
	::testing::InitGoogleTest( &argc, argv );

	g_PrimesTestEnvironment = dynamic_cast<PrimesTestEnvironment*>(::testing::AddGlobalTestEnvironment(new PrimesTestEnvironment(_max_number, _portion, _workers)));

	return RUN_ALL_TESTS();
}

