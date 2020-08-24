#pragma once

#include <masstransit_cpp/receive_endpoint_config_base.hpp>
#include <masstransit_cpp/host_info.hpp>
#include <chrono>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		struct MASSTRANSIT_CPP_API receive_endpoint_config : receive_endpoint_config_base
		{
			const uint16_t prefetch_count;
			const std::chrono::microseconds timeout;
			const host_info host;
			const bool exclusive;

			receive_endpoint_config(std::string const& queue, const size_t concurrency_limit, 
				std::chrono::microseconds const& timeout, const uint16_t prefetch_count,
			    host_info host, const bool exclusive);
		};
	}
}
