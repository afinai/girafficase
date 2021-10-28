#pragma once
#include <http/server/dispatcher.hpp>

namespace giraffic::http
{
	class handler : public Poco::Net::HTTPRequestHandler
	{
		public:
			explicit handler(const giraffic::http::dispatcher& dispatcher);
			virtual ~handler();

		public:
			void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;

		private:
			const giraffic::http::dispatcher& m_dispatcher;
	};
}

