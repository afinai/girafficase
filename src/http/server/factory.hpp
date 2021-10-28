#pragma once

#include <http/server/dispatcher.hpp>
#include <Poco/Net/HTTPRequestHandlerFactory.h>

namespace giraffic::http
{
	class factory final : public Poco::Net::HTTPRequestHandlerFactory
	{
		public:
		explicit factory(const giraffic::http::dispatcher& dispatcher);
		~factory() = default;
		
		private:
		Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override;
		
		private:
		giraffic::http::dispatcher m_dispatcher;
	};
}