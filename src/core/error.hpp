#pragma once
#include <core/headers.hpp>

namespace giraffic::core
{
	enum class errc
	{
		fail = 0,
		success = 1,
		not_found = 2,
		invalid_argument = 3,
		insufficient_resources = 4,
		not_implement = 5
	};

	using error = std::error_code;
	
	std::error_code make_error_code(giraffic::core::errc);
	
	class exception final : public std::exception
	{
	public:
		exception(const exception&) = default;
		exception(exception&&) = default;
		exception& operator=(const exception&) = default;
		exception& operator=(exception&&) = default;
		~exception() = default;

		exception(char const* const what, const giraffic::core::errc& ec);

		const char* what() const noexcept override;

		const giraffic::core::errc& code() const;

	private:
		std::string m_what;
		giraffic::core::errc m_errc;
	};
}

namespace std
{
  template <>
    struct is_error_code_enum<giraffic::core::errc> : true_type {};
}