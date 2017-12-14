#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API ssl_config
	{
		std::string path_to_ca_cert;
		std::string path_to_client_key;
		std::string path_to_client_cert;
		bool verify_hostname{ false };

		friend bool operator==(ssl_config const& lhs, ssl_config const& rhs);
		friend bool operator!=(ssl_config const& lhs, ssl_config const& rhs);
	};

	struct MASSTRANSIT_CPP_API amqp_host
	{
		explicit amqp_host(std::string const& host);
		amqp_host(std::string const& host, int port, std::string const& virtual_host,
			std::string const& user, std::string const& password,
			boost::optional<ssl_config> const& ssl);

		std::string host;
		std::string vhost{ "/" };
		int port{ 5672 };
		std::string user;
		std::string password;
		boost::optional<ssl_config> ssl;

		std::string to_string() const;  // "amqp://username:password@host/"

		boost::shared_ptr<AmqpClient::Channel> create_channel() const;
		
		static const std::string localhost;

		friend bool operator == (amqp_host const& l, amqp_host const& r);
		friend bool operator != (amqp_host const& l, amqp_host const& r);
	};
}
