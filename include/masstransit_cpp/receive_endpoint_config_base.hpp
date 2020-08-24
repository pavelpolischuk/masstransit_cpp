#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API receive_endpoint_config_base
	{
		const std::string queue;
		const size_t concurrency_limit;

		receive_endpoint_config_base(std::string queue, const size_t concurrency_limit)
			: queue(std::move(queue))
			, concurrency_limit(concurrency_limit > 0 ? concurrency_limit : 1)
		{}
	};
}
