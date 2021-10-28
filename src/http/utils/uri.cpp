#include "uri.hpp"

#include <Poco/StringTokenizer.h>

namespace giraffic::http
{
	uri::uri(const std::string& url)
	:Poco::URI(url)
	{
	}
	
	uri::~uri()
	{
	}
		
	std::vector<std::string> uri::getSegments()
	{
		std::vector<std::string> segments;
		getPathSegments(segments);

		return segments;
	}

	giraffic::http::params uri::getParams() const
	{		
		if (empty())
		{
			return giraffic::http::params{};
		}

		std::string query_str(getQuery());
		if ("" == query_str)	
		{
			return giraffic::http::params{};
		}

		std::map<std::string, std::string> _queries;
		
		Poco::StringTokenizer restTokenizer(query_str, "&");
		for (Poco::StringTokenizer::Iterator itr = restTokenizer.begin(); itr != restTokenizer.end(); ++itr)
		{
			Poco::StringTokenizer keyValueTokenizer(*itr, "=");
			_queries[keyValueTokenizer[0]] = (1 < keyValueTokenizer.count()) ? keyValueTokenizer[1] : "";
		}
		
		giraffic::http::params _params;
		_params.set(_queries);

		return _params;
	}
}