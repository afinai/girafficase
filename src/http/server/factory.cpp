#include "factory.hpp"
#include "handler.hpp"
#include <Poco/Net/HTTPServerRequest.h>

namespace giraffic::http
{
	factory::factory(const giraffic::http::dispatcher& dispatcher)
	:m_dispatcher(dispatcher)
	{
	}
	
	Poco::Net::HTTPRequestHandler* factory::createRequestHandler(const Poco::Net::HTTPServerRequest& request)
	{
		return new giraffic::http::handler(m_dispatcher);
	}
}