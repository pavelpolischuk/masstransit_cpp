#include "masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint_configurator::receive_endpoint_configurator(amqp_host const& host, std::string const& queue_name)
			: i_receive_endpoint_configurator(queue_name)
			, host_(host)
		{
		}

		receive_endpoint_configurator::~receive_endpoint_configurator() = default;

		receive_endpoint_configurator& receive_endpoint_configurator::auto_delete(const bool is)
		{
			auto_delete_ = is;
			return *this;
		}

		receive_endpoint_configurator& receive_endpoint_configurator::exclusive(bool is)
		{
			exclusive_ = is;
			return *this;
		}

		receive_endpoint_configurator& receive_endpoint_configurator::prefetch_count(const uint16_t count)
		{
			prefetch_count_ = count;
			return *this;
		}

		receive_endpoint_configurator& receive_endpoint_configurator::timeout(std::chrono::microseconds const& timeout)
		{
			timeout_ = timeout;
			return *this;
		}

		receive_endpoint::factory receive_endpoint_configurator::get_factory(std::shared_ptr<i_error_handler> const& error_handler) const
		{
			return [config = *this, error_handler](std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host)
			{
				return build(config, publish_endpoint, client_host, error_handler);
			};
		}

		std::shared_ptr<receive_endpoint> receive_endpoint_configurator::build(receive_endpoint_configurator const& configurator, std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host, std::shared_ptr<i_error_handler> const& error_handler)
		{
			auto channel = std::make_shared<rabbit_mq::amqp_channel>(configurator.host_);
			channel->declare_queue(configurator.queue_, false, true, configurator.exclusive_, configurator.auto_delete_);
			channel->declare_queue(receive_endpoint::get_error_queue(configurator.queue_), false, true, configurator.exclusive_, configurator.auto_delete_);
			return std::make_shared<receive_endpoint>(channel, 
				receive_endpoint_config
				{ 
					configurator.queue_, 
					configurator.concurrency_limit_, 
					configurator.timeout_, 
					configurator.prefetch_count_, 
					client_host,
					configurator.exclusive_ 
				},
				configurator.create_consumers(), 
				publish_endpoint, error_handler);
		}
	}
}
