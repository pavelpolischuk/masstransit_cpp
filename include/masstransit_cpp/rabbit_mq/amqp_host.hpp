#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_EXPORT amqp_uri
	{
		explicit amqp_uri(std::string const& host);

		std::string host;

		std::string to_string() const;  // "amqp://host"

		static const amqp_uri localhost;
	};

	struct MASSTRANSIT_CPP_EXPORT amqp_host
	{
		explicit amqp_host(amqp_uri const& uri, std::string const& user = "", std::string const& password = "");

		amqp_uri uri;
		std::string user;
		std::string password;

		std::string to_string() const;  // "amqp://username:password@host"

		friend bool operator == (amqp_host const& l, amqp_host const& r);
	};

	class MASSTRANSIT_CPP_EXPORT amqp_host_configurator
	{
	public:
		explicit amqp_host_configurator(amqp_uri const& uri);

		amqp_host_configurator & username(std::string const& username);
		amqp_host_configurator & password(std::string const& password);

		amqp_host get_host() const;

	private:
		amqp_uri uri_;
		std::string username_;
		std::string password_;
	};
}