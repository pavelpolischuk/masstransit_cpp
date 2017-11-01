#include "masstransit_cpp/rabbit_mq/amqp_host.hpp"
#include <sstream>

namespace masstransit_cpp
{
	const std::string amqp_host::localhost{ "127.0.0.1" };

	amqp_host::amqp_host(std::string const& host, int port, std::string const& user, std::string const& password)
		: host(host)
		, port(port)
		, user(user)
		, password(password)
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

		res << host;
		return res.str();
	}
	
	amqp_host_configurator::amqp_host_configurator(std::string const& host)
		: host_(host)
	{
	}

	amqp_host_configurator& amqp_host_configurator::port(int port)
	{
		port_ = port;
		return *this;
	}

	amqp_host_configurator& amqp_host_configurator::username(std::string const& username)
	{
		username_ = username;
		return *this;
	}

	amqp_host_configurator& amqp_host_configurator::password(std::string const& password)
	{
		password_ = password;
		return *this;
	}

	amqp_host amqp_host_configurator::get_host() const
	{
		return amqp_host{ host_, port_, username_, password_ };
	}

	bool operator==(amqp_host const& l, amqp_host const& r)
	{
		return l.host == r.host
			&& l.port == r.port
			&& l.user == r.user
			&& l.password == r.password;
	}

	bool operator!=(amqp_host const& l, amqp_host const& r)
	{
		return !(l == r);
	}
}
