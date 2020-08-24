#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/ssl_config.hpp>

#include <string>
#include <optional>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API amqp_host
	{
		explicit amqp_host(std::string const& host);
		
		amqp_host(std::string const& host, std::string const& user, std::string const& password);

		amqp_host(std::string const& host, int port, std::string const& virtual_host,
		          std::string const& user, std::string const& password, std::optional<ssl_config> const& ssl);

		std::string host;
		std::string vhost{"/"};
		int port{5672};
		std::string user;
		std::string password;
		std::optional<ssl_config> ssl;

		std::string to_string() const; // "amqp://username:password@host/"

		static const std::string LOCALHOST;

		friend bool MASSTRANSIT_CPP_API operator ==(amqp_host const& l, amqp_host const& r);
		friend bool MASSTRANSIT_CPP_API operator !=(amqp_host const& l, amqp_host const& r);
	};
}
