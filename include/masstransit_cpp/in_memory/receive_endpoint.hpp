#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>
#include <masstransit_cpp/in_memory/receive_endpoint_config.hpp>
#include <masstransit_cpp/threads/worker_thread.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class MASSTRANSIT_CPP_API receive_endpoint : public i_receive_endpoint
		{
		public:
			using factory = std::function<std::shared_ptr<receive_endpoint>(std::shared_ptr<i_publish_endpoint> const& publish_endpoint)>;
			
			receive_endpoint(receive_endpoint_config const& config, 
				consumers_map const& consumers_by_type, 
				std::shared_ptr<i_publish_endpoint> const& publish_endpoint, 
				std::shared_ptr<i_error_handler> const& error_handler);

			std::future<void> deliver(consume_context_info const& context);

		private:
			const receive_endpoint_config config_;
			threads::worker_thread consumer_worker_;
		};
	}
}
