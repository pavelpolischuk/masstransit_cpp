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

		receive_endpoint_configurator::~receive_endpoint_configurator()
		{
		}

		receive_endpoint_configurator& receive_endpoint_configurator::transport_concurrency_limit(size_t limit)
		{
			transport_concurrency_limit_ = limit;
			return *this;
		}

		receive_endpoint::builder receive_endpoint_configurator::get_builder() const
		{
			return bind(&build, *this);
		}

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build(receive_endpoint_configurator configuration)
		{
			return std::make_shared<receive_endpoint>(configuration.queue_, configuration.create_consumers());
		}
	}
}
