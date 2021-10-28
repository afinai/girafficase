#pragma once
#include <core/headers.hpp>
#include <core/logger.hpp>

namespace giraffic
{
	namespace primes
	{
		class detector final
		{
			public:
			detector(std::uint64_t max_number, std::uint64_t portion, std::size_t workers, std::shared_ptr<giraffic::core::logger> logger);
			~detector();

			bool is_prime(std::uint64_t number);
			void build();

			private:
				void build_for(std::uint64_t number);
				bool is_prime_for(std::uint64_t number, std::uint64_t start, std::uint64_t end);

			private:
				std::shared_ptr<giraffic::core::logger> m_logger;
				std::vector<bool> m_numbers;
				std::uint64_t m_max_number;
				std::uint64_t m_portion;
				std::size_t m_workers;
				std::atomic<std::size_t> m_async_count;
		};
	}
}