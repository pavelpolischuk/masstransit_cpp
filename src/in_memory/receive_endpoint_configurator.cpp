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

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build()
		{
			return nullptr;
		}
	}
}
