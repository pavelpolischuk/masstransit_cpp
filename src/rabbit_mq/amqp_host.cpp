#include "masstransit_cpp/rabbit_mq/amqp_host.hpp"
#include <sstream>

namespace masstransit_cpp
{
	amqp_uri::amqp_uri(std::string const& host)
		: host(host)
	{
	}
	
	std::string amqp_uri::to_string() const
	{
		return "amqp://" + host;
	}

	const amqp_uri amqp_uri::localhost{ "127.0.0.1" };

	amqp_host::amqp_host(amqp_uri const& uri, std::string const& user, std::string const& password)
		: uri(uri)
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

		res << uri.host;
		return res.str();
	}

	amqp_host_configurator::amqp_host_configurator(amqp_uri const& uri)
		: uri_(uri)
	{
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
		return amqp_host{ uri_, username_, password_ };
	}

	bool operator==(amqp_host const& l, amqp_host const& r)
	{
		return l.uri.host == r.uri.host
			&& l.user == r.user
			&& l.password == r.password;
	}
}
