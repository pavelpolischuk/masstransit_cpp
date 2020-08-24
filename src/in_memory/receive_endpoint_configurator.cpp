#include "masstransit_cpp/in_memory/receive_endpoint_configurator.hpp"
#include "masstransit_cpp/in_memory/receive_endpoint.hpp"

namespace masstransit_cpp
{
	namespace in_memory
	{
		receive_endpoint_configurator::receive_endpoint_configurator(std::string const& queue_name)
			: i_receive_endpoint_configurator(queue_name)
		{
		}

		receive_endpoint_configurator::~receive_endpoint_configurator() = default;

		receive_endpoint_configurator& receive_endpoint_configurator::transport_concurrency_limit(const size_t limit)
		{
			transport_concurrency_limit_ = limit;
			return *this;
		}

		receive_endpoint::factory receive_endpoint_configurator::get_factory(std::shared_ptr<i_error_handler> const& error_handler) const
		{
			return [config = *this, error_handler](std::shared_ptr<i_publish_endpoint> const& publish_endpoint)
			{
				return build(config, publish_endpoint, error_handler);
			};
		}

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build(receive_endpoint_configurator const& configuration, std::shared_ptr<i_publish_endpoint> const& publish_endpoint, std::shared_ptr<i_error_handler> const& error_handler)
		{
			return std::make_shared<receive_endpoint>(
				receive_endpoint_config
				{ 
					configuration.queue_, 
					configuration.concurrency_limit_ 
				},
				configuration.create_consumers(),
				publish_endpoint,
				error_handler);
		}
	}
}
