#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>

#include <memory>

namespace masstransit_cpp
{
	class bus;
	
	class MASSTRANSIT_CPP_API rabbit_mq_configurator
	{
	public:
		explicit rabbit_mq_configurator();

		amqp_host host(std::string const& host, std::function<void(amqp_host_configurator&)> const& configure);
		rabbit_mq_configurator & receive_endpoint(amqp_host const& host, std::string const& queue_name, std::function<void(rabbit_mq::receive_endpoint_configurator&)> const& configure);

		rabbit_mq_configurator & client(host_info const& client_info);
		rabbit_mq_configurator & auto_delete(bool is);

		std::shared_ptr<bus> build() const;
	
	private:
		std::map<std::string, rabbit_mq::receive_endpoint_configurator> receive_endpoints_;
		bool auto_delete_{ false };
		host_info client_info_;
		amqp_host host_;
		boost::optional<ssl_config> ssl_;
	};
}
