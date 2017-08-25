#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT in_memory_bus : public bus
	{
	public:
		in_memory_bus();
		~in_memory_bus() override;

	protected:
		void run() override;

		void publish_impl(consume_context_info const& message, std::string const& type) const override;
	};
}