#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"

#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint::receive_endpoint(boost::shared_ptr<AmqpClient::Channel> const& channel, std::string const& queue, uint16_t prefetch_count,
			boost::posix_time::time_duration const& timeout, consumers_map const& consumers_by_type)
			: queue_(queue)
			, prefetch_count_(prefetch_count)
			, timeout_ms_(get_ms(timeout))
			, channel_(channel)
			, consumers_by_type_(consumers_by_type)
		{
		}

		bool receive_endpoint::try_consume() const
		{
			AmqpClient::Envelope::ptr_t envelope;
			if (!channel_->BasicConsumeMessage(tag_, envelope, timeout_ms_))
				return false;

			if (envelope == nullptr)
				return false;

			auto message = envelope->Message();
			auto body = message->Body();
			auto body_j = nlohmann::json::parse(body.begin(), body.end());
			consume_context_info context(body_j);

			auto consumer = find_consumer(context.message_types);
			if (consumer == nullptr)
				return true;

			try
			{
				BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;

				consumer->consume(context);
				channel_->BasicAck(envelope);

				BOOST_LOG_TRIVIAL(debug) << "[DONE]";
			}
			catch (std::exception & ex)
			{
				BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
					<< body << "\n\tException: " << ex.what();
			}
			catch (...)
			{
				BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
					<< body << "\n\tException: unknown";
			}

			return true;
		}

		void receive_endpoint::bind_queues(std::shared_ptr<exchange_manager> const& exchanges)
		{
			for (auto const& c : consumers_by_type_)
			{
				auto type = c.second->message_type();
				exchanges->declare_message_type(type, channel_);
				channel_->BindQueue(queue_, type);
			}

			tag_ = channel_->BasicConsume(queue_, "", true, false, true, prefetch_count_);
		}

		std::shared_ptr<i_message_consumer> receive_endpoint::find_consumer(std::vector<std::string> const& message_types) const
		{
			for (auto const& message_type : message_types)
			{
				auto consumer = consumers_by_type_.find(message_type);
				if (consumer != consumers_by_type_.end())
					return consumer->second;
			}

			return nullptr;
		}

		int receive_endpoint::get_ms(boost::posix_time::time_duration const& timeout)
		{
			auto ms = timeout.total_milliseconds();
			if (ms <= static_cast<int64_t>(std::numeric_limits<int>::max()))
				return static_cast<int>(ms);
			
			BOOST_LOG_TRIVIAL(warning) << "receive_endpoint::ctor: ms count is greater numeric_limits<int>::max";
			return 500;
		}
	}
}