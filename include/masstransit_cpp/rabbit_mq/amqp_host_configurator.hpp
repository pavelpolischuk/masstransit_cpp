#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>

#include <string>
#include <functional>
#include <boost/optional.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API amqp_host_configurator
	{
	public:
		explicit amqp_host_configurator(std::string const& host_address, std::string const& virtual_host = "/");

		amqp_host_configurator & port(int port);
		amqp_host_configurator & username(std::string const& username);
		amqp_host_configurator & password(std::string const& password);
		amqp_host_configurator & use_ssl(std::function<void(ssl_config&)> const& configure);

		amqp_host get_host() const;

	private:
		std::string host_;
		std::string vhost_;
		int port_{ 5672 };
		std::string username_;
		std::string password_;

		boost::optional<ssl_config> ssl_;
	};
}
