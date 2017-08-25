#include "masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_host.hpp"
#include "masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"

namespace masstransit_cpp
{
	using namespace rabbit_mq;

	rabbit_mq_configurator::rabbit_mq_configurator()
		: host_(amqp_uri::localhost)
	{
	}

	amqp_host rabbit_mq_configurator::host(amqp_uri const& uri, std::function<void(amqp_host_configurator&)> const& configure)
	{
		amqp_host_configurator configurator(uri);
		configure(configurator);
		return host_ = configurator.get_host();
	}

	rabbit_mq_configurator& rabbit_mq_configurator::receive_endpoint(amqp_host const& host, std::string const& queue_name, std::function<void(receive_endpoint_configurator&)> const& configure)
	{
		auto key = host.uri.host + queue_name;
		auto q = receive_endpoints_.find(key);
		if (q != receive_endpoints_.end())
		{
			configure(q->second);
			return *this;
		}

		auto inserted = receive_endpoints_.insert({ key, { host, queue_name } });
		if (inserted.second)
		{
			configure(inserted.first->second);
		}

		return *this;
	}

	rabbit_mq_configurator& rabbit_mq_configurator::client(host_info const& info)
	{
		client_info_ = info;
		return *this;
	}

	rabbit_mq_configurator& rabbit_mq_configurator::auto_delete(bool is)
	{
		auto_delete_ = is;
		return *this;
	}

	std::shared_ptr<bus> rabbit_mq_configurator::build()
	{
		auto exchanges = std::make_shared<exchange_manager>();
		
		std::vector<std::shared_ptr<rabbit_mq::receive_endpoint>> receivers;
		for(auto & receive_factory : receive_endpoints_)
		{
			auto receiver = receive_factory.second.build();
			receiver->bind_queues(exchanges);
			receivers.push_back(receiver);
		}

		return std::make_shared<rabbit_mq_bus>(host_, client_info_, exchanges, receivers);
	}
}
