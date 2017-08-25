#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/message_consumer.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	class exchange_manager;

	namespace rabbit_mq
	{
		class MASSTRANSIT_CPP_EXPORT receive_endpoint
		{
		public:
			using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;

			receive_endpoint(boost::shared_ptr<AmqpClient::Channel> const& channel, std::string const& queue, uint16_t prefetch_count, boost::posix_time::time_duration const& timeout, consumers_map const& consumers_by_type);

			bool try_consume() const;
			void bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager);

		private:
			const std::string queue_;
			const uint16_t prefetch_count_;
			const int timeout_ms_;

			boost::shared_ptr<AmqpClient::Channel> channel_;
			consumers_map consumers_by_type_;
			std::string tag_;

			std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;

			static int get_ms(boost::posix_time::time_duration const& timeout);
		};
	}
}