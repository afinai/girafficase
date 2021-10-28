#pragma once

#include <core/headers.hpp>
#include <http/utils/params.hpp>

namespace giraffic::http
{
	using executer = std::function <giraffic::core::errc(const Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse&, const giraffic::http::params& params)>;
}