#pragma once

#include <masstransit_cpp/global.hpp>
#include <memory>
#include <functional>

namespace masstransit_cpp
{
	class bus;
	class in_memory_configurator;
	class rabbit_mq_configurator;

	class MASSTRANSIT_CPP_API bus_factory
	{
	public:
		static std::shared_ptr<bus> create_using_rabbit_mq(std::function<void(rabbit_mq_configurator & configurator)> const& configure);
		
		static std::shared_ptr<bus> create_using_in_memory(std::function<void(in_memory_configurator & configurator)> const& configure);
	};
}
