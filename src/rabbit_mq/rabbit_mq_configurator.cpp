#include "masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_host_configurator.hpp"
#include "masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"

namespace masstransit_cpp
{
	using namespace rabbit_mq;

	rabbit_mq_configurator::rabbit_mq_configurator()
		: host_(amqp_host::localhost)
	{
	}

	amqp_host rabbit_mq_configurator::host(std::string const& host, std::function<void(amqp_host_configurator&)> const& configure)
	{
		amqp_host_configurator configurator(host);
		configure(configurator);
		return host_ = configurator.get_host();
	}

	rabbit_mq_configurator& rabbit_mq_configurator::receive_endpoint(amqp_host const& host, std::string const& queue_name, std::function<void(receive_endpoint_configurator&)> const& configure)
	{
		const auto key = host.host + queue_name;
		auto q = receive_endpoints_.find(key);
		if (q != receive_endpoints_.end())
		{
			configure(q->second);
			return *this;
		}

		auto inserted = receive_endpoints_.insert({ key, receive_endpoint_configurator{ host, queue_name } });
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

	rabbit_mq_configurator& rabbit_mq_configurator::auto_delete(const bool is)
	{
		auto_delete_ = is;
		return *this;
	}

	std::shared_ptr<bus> rabbit_mq_configurator::build() const
	{
		std::vector<receive_endpoint::factory> receivers_factories;
		for(auto & receive_factory : receive_endpoints_)
		{
			receivers_factories.push_back(receive_factory.second.get_factory());
		}

		const auto exchanges = std::make_shared<exchange_manager>(auto_delete_);
		return std::shared_ptr<rabbit_mq_bus>(new rabbit_mq_bus(host_, client_info_, exchanges, receivers_factories));
	}
}
