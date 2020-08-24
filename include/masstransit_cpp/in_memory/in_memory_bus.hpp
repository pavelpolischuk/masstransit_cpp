#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint.hpp>

namespace masstransit_cpp
{
	class bus_factory;
	class i_error_handler;

	class MASSTRANSIT_CPP_API in_memory_bus : public bus, public std::enable_shared_from_this<in_memory_bus>
	{
	public:
		~in_memory_bus() override;

		void start() override;
		void wait() const override;
		void stop() override;
		
	protected:
		in_memory_bus(std::vector<in_memory::receive_endpoint::factory> const& receivers_factories, std::shared_ptr<i_error_handler> error_handler);
	
		std::future<bool> publish(consume_context_info message, std::string const& type) const override;

	friend class in_memory_configurator;
	
	private:
		const std::vector<in_memory::receive_endpoint::factory> receivers_factories_;
		const std::shared_ptr<i_error_handler> error_handler_;
		
		std::vector<std::shared_ptr<in_memory::receive_endpoint>> receivers_;
		std::shared_ptr<threads::worker_thread> publish_worker_;
	};
}
