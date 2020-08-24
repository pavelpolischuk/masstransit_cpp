#include "masstransit_cpp/rabbit_mq/receive_endpoint_config.hpp"

#include <utility>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint_config::receive_endpoint_config(std::string const& queue, const size_t concurrency_limit,
		                                                 std::chrono::microseconds const& timeout,
		                                                 const uint16_t prefetch_count, host_info host,
		                                                 const bool exclusive)
			: receive_endpoint_config_base(queue, concurrency_limit)
			, prefetch_count(prefetch_count)
			, timeout(timeout)
			, host(std::move(host))
			, exclusive(exclusive)
		{
		}
	}
}
