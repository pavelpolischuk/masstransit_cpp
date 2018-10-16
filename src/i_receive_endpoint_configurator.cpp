#include "masstransit_cpp/i_receive_endpoint_configurator.hpp"

namespace masstransit_cpp
{
	i_receive_endpoint_configurator::i_receive_endpoint_configurator(std::string const& queue_name)
		: queue_(queue_name)
	{
	}
	
	void i_receive_endpoint_configurator::add_consumer_factory(std::string const& message_type, std::function<std::shared_ptr<i_message_consumer>()> const& factory)
	{
		consumers_factories_by_type_["urn:message:" + message_type] = factory;
	}

	auto i_receive_endpoint_configurator::create_consumers() const -> consumers_map
	{
		consumers_map consumers;
		for (auto const& f : consumers_factories_by_type_)
			consumers.insert({ f.first, f.second() });

		return std::move(consumers);
	}
}
