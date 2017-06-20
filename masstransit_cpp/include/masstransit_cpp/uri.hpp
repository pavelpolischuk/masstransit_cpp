#pragma once
#include <masstransit_cpp/global.hpp>
#include <string>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_EXPORT uri
	{
		uri() {}
		explicit uri(std::string const& host, std::string const& user = "", std::string const& password = "");

		std::string host;
		std::string user;
		std::string password;

		std::string to_string() const;  // "amqp://username:password@host"
	};
}
