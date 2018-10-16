#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint_configurator.hpp>

#include <memory>
#include <functional>

namespace masstransit_cpp
{
	class bus;

	class MASSTRANSIT_CPP_API in_memory_configurator
	{
	public:
		in_memory_configurator & receive_endpoint(std::string const& queue_name, std::function<void(in_memory::receive_endpoint_configurator&)> const& configure);

		std::shared_ptr<bus> build() const;
	
	private:
		std::map<std::string, in_memory::receive_endpoint_configurator> receive_endpoints_;
	};
}
