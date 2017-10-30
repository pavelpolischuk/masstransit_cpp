#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/shared_ptr.hpp>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class exchange_manager;

		class MASSTRANSIT_CPP_API receive_endpoint : public i_receive_endpoint
		{
		public:
			using factory = std::function<std::shared_ptr<receive_endpoint>()>;

			receive_endpoint(boost::shared_ptr<AmqpClient::Channel> const& channel, std::string const& queue, uint16_t prefetch_count, boost::posix_time::time_duration const& timeout, consumers_map const& consumers_by_type);

			bool try_consume() const;
			void bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager);

		private:
			const std::string queue_;
			const uint16_t prefetch_count_;
			const int timeout_ms_;

			boost::shared_ptr<AmqpClient::Channel> channel_;
			std::string tag_;

			static int get_ms(boost::posix_time::time_duration const& timeout);
		};
	}
}