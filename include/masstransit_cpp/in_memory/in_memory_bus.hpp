#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API in_memory_bus : public bus
	{
	public:
		in_memory_bus(std::vector<in_memory::receive_endpoint::factory> const& receivers_factories);
		~in_memory_bus() override;

		void start() override;
		void stop() override;
		
	protected:
		std::future<bool> publish_impl(consume_context_info const& message, std::string const& type) const override;

	private:
		const std::vector<in_memory::receive_endpoint::factory> receivers_factories_;
		std::vector<std::shared_ptr<in_memory::receive_endpoint>> receivers_;
		std::unique_ptr<threads::worker_thread> publish_worker_;
	};
}
