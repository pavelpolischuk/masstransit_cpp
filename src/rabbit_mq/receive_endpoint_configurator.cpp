#include "masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp"

#include <SimpleAmqpClient/Channel.h>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint_configurator::receive_endpoint_configurator(amqp_host const& host, std::string const& queue_name)
			: i_receive_endpoint_configurator(queue_name)
			, host_(host)
		{
		}

		receive_endpoint_configurator::~receive_endpoint_configurator()
		{
		}

		receive_endpoint_configurator& receive_endpoint_configurator::auto_delete(bool is)
		{
			auto_delete_ = is;
			return *this;
		}

		receive_endpoint_configurator& receive_endpoint_configurator::prefetch_count(uint16_t count)
		{
			prefetch_count_ = count;
			return *this;
		}

		receive_endpoint_configurator& receive_endpoint_configurator::poll_timeout(boost::posix_time::time_duration const& timeout)
		{
			timeout_ = timeout;
			return *this;
		}

		receive_endpoint::factory receive_endpoint_configurator::get_factory() const
		{
			return [config = *this](std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host)
			{
				return build(config, publish_endpoint, client_host);
			};
		}

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build(receive_endpoint_configurator configurator, std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host)
		{
			auto channel = configurator.host_.create_channel();
			channel->DeclareQueue(configurator.queue_, false, true, false, configurator.auto_delete_);
			channel->DeclareQueue(receive_endpoint::get_error_queue(configurator.queue_), false, true, false, configurator.auto_delete_);
			return std::make_shared<receive_endpoint>(channel, configurator.queue_, client_host, configurator.prefetch_count_, configurator.timeout_, configurator.create_consumers(), publish_endpoint);
		}
	}
}
