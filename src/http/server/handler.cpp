#include "handler.hpp"

namespace giraffic::http
{
	handler::handler(const giraffic::http::dispatcher& dispatcher)
	:m_dispatcher(dispatcher)
	{
	}
	
	handler::~handler()
	{
	}

	void handler::handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response)
	{
		auto errc = m_dispatcher.execute(request, response);
		
		if (response.getKeepAlive())
		{
			Poco::NullOutputStream nullStream;
			Poco::StreamCopier::copyStream(request.stream(), nullStream);
		}
		
		if(errc != giraffic::core::errc::success)
		{
			response.setContentLength(0);
			response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_IMPLEMENTED);
			response.send();
		}
	}
}

