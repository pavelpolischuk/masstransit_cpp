#include "masstransit_cpp/rabbit_mq/amqp_host.hpp"

#include <sstream>

namespace masstransit_cpp
{
	const std::string amqp_host::LOCALHOST{"127.0.0.1"};

	amqp_host::amqp_host(std::string const& host)
		: host(host)
	{
	}

	amqp_host::amqp_host(std::string const& host, std::string const& user, std::string const& password)
		: host(host)
		, user(user)
		, password(password)
	{
	}

	amqp_host::amqp_host(std::string const& host, const int port, std::string const& virtual_host,
	                     std::string const& user, std::string const& password, std::optional<ssl_config> const& ssl)
		: host(host)
		, vhost(virtual_host)
		, port(port)
		, user(user)
		, password(password)
		, ssl(ssl)
	{
	}

	std::string amqp_host::to_string() const
	{
		std::stringstream res;
		res << "amqp://";

		if (!user.empty())
		{
			res << user;
			if (!password.empty()) res << ':' << password;
			res << '@';
		}

		res << host << vhost;
		return res.str();
	}

	bool operator==(amqp_host const& l, amqp_host const& r)
	{
		return l.host == r.host
			&& l.vhost == r.vhost
			&& l.port == r.port
			&& l.user == r.user
			&& l.password == r.password
			&& l.ssl == r.ssl;
	}

	bool operator!=(amqp_host const& l, amqp_host const& r)
	{
		return !(l == r);
	}
}
