#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_receive_endpoint_configurator.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class receive_endpoint;

		class MASSTRANSIT_CPP_EXPORT receive_endpoint_configurator : public i_receive_endpoint_configurator
		{
		public:
			receive_endpoint_configurator(std::string const& queue_name);
			~receive_endpoint_configurator() override;

			std::shared_ptr<receive_endpoint> build();
		};
	}
}
