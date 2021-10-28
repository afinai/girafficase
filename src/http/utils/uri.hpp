#pragma once
#include <core/headers.hpp>
#include <http/utils/params.hpp>

namespace giraffic::http
{
	class uri final : public Poco::URI
	{
		public:
		explicit uri(const std::string& url);
		~uri();
		
		std::vector<std::string> getSegments();
		giraffic::http::params getParams() const;
	};
}