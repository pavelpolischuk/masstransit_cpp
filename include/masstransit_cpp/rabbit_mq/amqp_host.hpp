#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>

namespace masstransit_cpp
{

	struct MASSTRANSIT_CPP_API amqp_host
	{
		explicit amqp_host(std::string const& host, int port = 5672, std::string const& user = "", std::string const& password = "");

		std::string host;
		int port;
		std::string user;
		std::string password;

		std::string to_string() const;  // "amqp://username:password@host"
		
		static const std::string localhost;

		friend bool operator == (amqp_host const& l, amqp_host const& r);
		friend bool operator != (amqp_host const& l, amqp_host const& r);
	};

	class MASSTRANSIT_CPP_API amqp_host_configurator
	{
	public:
		explicit amqp_host_configurator(std::string const& host);

		amqp_host_configurator & port(int port);
		amqp_host_configurator & username(std::string const& username);
		amqp_host_configurator & password(std::string const& password);

		amqp_host get_host() const;

	private:
		std::string host_;
		int port_{ 5672 };
		std::string username_;
		std::string password_;
	};
}