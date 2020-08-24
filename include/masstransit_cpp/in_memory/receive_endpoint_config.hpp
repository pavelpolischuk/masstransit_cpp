#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/receive_endpoint_config_base.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		struct MASSTRANSIT_CPP_API receive_endpoint_config : receive_endpoint_config_base
		{
			receive_endpoint_config(std::string const& queue, const size_t concurrency_limit)
				: receive_endpoint_config_base(queue, concurrency_limit)
			{
			}
		};
	}
}
