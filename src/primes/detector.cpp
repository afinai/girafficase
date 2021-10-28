#include "detector.hpp"

namespace giraffic::primes
{
	detector::detector(std::uint64_t max_number, std::uint64_t portion, std::size_t workers, std::shared_ptr<giraffic::core::logger> logger)
		:m_max_number(max_number), m_portion(portion), m_workers(workers), m_logger(logger), m_numbers(max_number + 1, true), m_async_count(0)
	{
		
	}
	
	detector::~detector()
	{

	}

	void detector::build()
	{
		std::vector<std::future<void>> _asyncs;

		for (std::uint64_t i = 2; i * i <= m_max_number; i++)
		{
			if (m_numbers[i] == true)
			{
				//build_for(i);

				_asyncs.emplace_back(std::async(std::launch::async, &detector::build_for, this, i));
			}
		}
	}

	void detector::build_for(std::uint64_t number)
	{
		++m_async_count;

		for (std::uint64_t j = number * 2; j <= m_max_number; j += number)
		{
			m_numbers[j] = false;
		}

		--m_async_count;
	}

	bool detector::is_prime(std::uint64_t number)
	{
		return is_prime_for(number, 2, m_max_number);
	}

	bool detector::is_prime_for(std::uint64_t number, std::uint64_t start, std::uint64_t end)
	{
		std::uint64_t _len = end - start;

		if(_len < m_portion || m_async_count >= m_workers*std::thread::hardware_concurrency())
		{
			for (int i = start; i <= end; i++)
			{
				if (m_numbers[i])
				{
					if (i == number)
					{
						return true;
					}
				}
			}

			return false;
		}

		std::uint64_t _mid = start + _len/2;

		++m_async_count;

		auto _handle = std::async(std::launch::async, &detector::is_prime_for, this, number, _mid, end);

		auto _isprime = is_prime_for(number, start, _mid);

		_isprime = _isprime + _handle.get();

		--m_async_count;

		return _isprime;
	}
}