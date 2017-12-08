#include "masstransit_cpp/rabbit_mq/amqp_host.hpp"
#include <SimpleAmqpClient/Channel.h>
#include <sstream>

namespace masstransit_cpp
{
	const std::string amqp_host::localhost{ "127.0.0.1" };

	amqp_host::amqp_host(std::string const& host, int port, 
		std::string const& user, std::string const& password,
		boost::optional<ssl_config> const& ssl)
		: host(host)
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

		res << host;
		return res.str();
	}

	boost::shared_ptr<AmqpClient::Channel> amqp_host::create_channel() const
	{
		if (!ssl)
			return AmqpClient::Channel::Create(host, port, user, password);
		
		auto ssl_ = ssl.get();
		return AmqpClient::Channel::CreateSecure(ssl_.path_to_ca_cert, host, 
			ssl_.path_to_client_key, ssl_.path_to_client_cert, port, user, password);
	}

	bool operator==(ssl_config const& lhs, ssl_config const& rhs)
	{
		return lhs.path_to_ca_cert == rhs.path_to_ca_cert
			&& lhs.path_to_client_key == rhs.path_to_client_key
			&& lhs.path_to_client_cert == rhs.path_to_client_cert
			&& lhs.verify_hostname == rhs.verify_hostname;
	}

	bool operator!=(ssl_config const& lhs, ssl_config const& rhs)
	{
		return !(lhs == rhs);
	}

	bool operator==(amqp_host const& l, amqp_host const& r)
	{
		return l.host == r.host
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
