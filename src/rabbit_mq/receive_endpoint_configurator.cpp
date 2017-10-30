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
			return bind(&build, *this);
		}

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build(receive_endpoint_configurator configurator)
		{
			auto channel = AmqpClient::Channel::CreateFromUri(configurator.host_.to_string());
			channel->DeclareQueue(configurator.queue_, false, true, false, configurator.auto_delete_);
			return std::make_shared<receive_endpoint>(channel, configurator.queue_, configurator.prefetch_count_, configurator.timeout_, configurator.create_consumers());
		}
	}
}