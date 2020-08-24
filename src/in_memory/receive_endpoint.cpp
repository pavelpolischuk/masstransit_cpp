#include "masstransit_cpp/in_memory/receive_endpoint.hpp"

namespace masstransit_cpp
{
	namespace in_memory
	{
		receive_endpoint::receive_endpoint(receive_endpoint_config const& config,
			consumers_map const& consumers_by_type, 
			std::shared_ptr<i_publish_endpoint> const& publish_endpoint, 
			std::shared_ptr<i_error_handler> const& error_handler)
			: i_receive_endpoint(consumers_by_type, publish_endpoint, error_handler)
			, config_(config)
		{
		}

		std::future<void> receive_endpoint::deliver(consume_context_info const& context)
		{
			return consumer_worker_.enqueue([this](consume_context_info const& message) {
				deliver_impl(message);
			}, context);
		}
	}
}
