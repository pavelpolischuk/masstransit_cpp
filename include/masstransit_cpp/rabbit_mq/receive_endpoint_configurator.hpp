#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_receive_endpoint_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <memory>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class receive_endpoint;

		class MASSTRANSIT_CPP_EXPORT receive_endpoint_configurator : public i_receive_endpoint_configurator
		{
		public:
			receive_endpoint_configurator(amqp_host const& host, std::string const& queue_name);
			~receive_endpoint_configurator() override;

			receive_endpoint_configurator & auto_delete(bool is);
			receive_endpoint_configurator & prefetch_count(uint16_t count);
			receive_endpoint_configurator & poll_timeout(boost::posix_time::time_duration const& timeout);

			std::shared_ptr<receive_endpoint> build();

		protected:
			bool auto_delete_{ false };
			uint16_t prefetch_count_{ 1 };
			boost::posix_time::time_duration timeout_{ boost::posix_time::milliseconds(500) };

			amqp_host host_;
		};
	}
}
