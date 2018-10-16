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

		auto inserted = receive_endpoints_.insert({ key, receive_endpoint_configurator{ queue_name } });
		if (inserted.second)
		{
			configure(inserted.first->second);
		}

		return *this;
	}

	std::shared_ptr<bus> in_memory_configurator::build() const
	{
		std::vector<receive_endpoint::factory> receivers_factories;
		for (auto const& receive_factory : receive_endpoints_)
		{
			receivers_factories.push_back(receive_factory.second.get_factory());
		}

		return std::shared_ptr<in_memory_bus>(new in_memory_bus(receivers_factories));
	}
}
