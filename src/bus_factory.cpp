#include "masstransit_cpp/bus_factory.hpp"
#include "masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp"
#include "masstransit_cpp/in_memory/in_memory_configurator.hpp"

namespace masstransit_cpp
{
	std::shared_ptr<bus> bus_factory::create_using_rabbit_mq(std::function<void(rabbit_mq_configurator & configurator)> const& configure)
	{
		rabbit_mq_configurator configurator;
		configure(configurator);
		return configurator.build();
	}

	std::shared_ptr<bus> bus_factory::create_using_in_memory(std::function<void(in_memory_configurator & configurator)> const& configure)
	{
		in_memory_configurator configurator;
		configure(configurator);
		return configurator.build();
	}
}
