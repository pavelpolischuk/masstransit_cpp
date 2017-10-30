#include "masstransit_cpp/in_memory/in_memory_configurator.hpp"
#include "masstransit_cpp/in_memory/in_memory_bus.hpp"

namespace masstransit_cpp
{
	using namespace in_memory;

	in_memory_configurator& in_memory_configurator::receive_endpoint(std::string const& queue_name, std::function<void(receive_endpoint_configurator&)> const& configure)
	{
		auto const& key = queue_name;
		auto q = receive_endpoints_.find(key);
		if (q != receive_endpoints_.end())
		{
			configure(q->second);
			return *this;
		}

		auto inserted = receive_endpoints_.insert({ key, { queue_name } });
		if (inserted.second)
		{
			configure(inserted.first->second);
		}

		return *this;
	}

	in_memory_configurator& in_memory_configurator::transport_concurrency_limit(size_t limit)
	{
		transport_concurrency_limit_ = limit;
		return *this;
	}

	std::shared_ptr<bus> in_memory_configurator::build()
	{
		std::map<std::string, receive_endpoint::factory> receivers_factories;
		for (auto const& receive_factory : receive_endpoints_)
		{
			receivers_factories.emplace(receive_factory.first, receive_factory.second.get_factory());
		}

		return std::make_shared<in_memory_bus>(receivers_factories);
	}
}