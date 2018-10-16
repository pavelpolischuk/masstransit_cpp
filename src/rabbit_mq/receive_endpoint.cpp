#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/datetime.hpp"

#include <stdexcept>
#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint::receive_endpoint(boost::shared_ptr<AmqpClient::Channel> const& channel, 
			std::string const& queue, host_info const& host,
		    const uint16_t prefetch_count, boost::posix_time::time_duration const& timeout,
			consumers_map const& consumers_by_type,
			std::shared_ptr<i_publish_endpoint> const& publish_endpoint)
			: i_receive_endpoint(consumers_by_type, publish_endpoint)
			, queue_(queue)
			, host_(host)
			, prefetch_count_(prefetch_count)
			, timeout_ms_(get_ms(timeout))
			, channel_(channel)
		{
		}

		bool receive_endpoint::try_consume() const
		{
			AmqpClient::Envelope::ptr_t envelope;
			if (!channel_->BasicConsumeMessage(tag_, envelope, timeout_ms_))
				return false;

			if (envelope == nullptr)
				return false;

			const auto message = envelope->Message();
			auto body = message->Body();
			auto body_j = nlohmann::json::parse(body.begin(), body.end());
			const auto context = body_j.get<consume_context_info>();

			auto consumer = find_consumer(context.message_types);
			if (consumer == nullptr)
				return true;

			try
			{
				BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;

				consumer->consume(context, publish_endpoint_);

				BOOST_LOG_TRIVIAL(debug) << "[DONE]";
			}
			catch (std::exception & ex)
			{
				on_error(context, consumer->message_type(), body, ex);
			}
			catch (...)
			{
				on_error(context, consumer->message_type(), body, std::runtime_error("unknown"));
			}

			channel_->BasicAck(envelope);
			return true;
		}

		void receive_endpoint::bind_queues(std::shared_ptr<exchange_manager> const& exchanges)
		{
			const auto error_queue = get_error_queue(queue_);

			exchanges->declare_exchange(queue_, channel_);
			exchanges->declare_exchange(error_queue, channel_);
			
			for (auto const& c : consumers_by_type_)
			{
				const auto type = c.second->message_type();
				exchanges->declare_exchange(type, channel_);
				channel_->BindExchange(queue_, type, "");
			}

			channel_->BindQueue(error_queue, error_queue);
			channel_->BindQueue(queue_, queue_);

			tag_ = channel_->BasicConsume(queue_, "", true, false, true, prefetch_count_);
		}

		int receive_endpoint::get_ms(boost::posix_time::time_duration const& timeout)
		{
			const auto ms = timeout.total_milliseconds();
			if (ms <= static_cast<int64_t>(std::numeric_limits<int>::max()))
				return static_cast<int>(ms);
			
			BOOST_LOG_TRIVIAL(warning) << "receive_endpoint::ctor: ms count is greater numeric_limits<int>::max";
			return 500;
		}

		void receive_endpoint::on_error(consume_context_info context, std::string const& consumer_type, 
			std::string const& message, std::exception const& ex) const
		{
			BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer_type << "] try handle message:\n"
				<< message << "\n\tException: " << ex.what();

			context.headers.emplace("MT-Fault-Message", std::string(ex.what()));
			context.headers.emplace("MT-Fault-Timestamp", to_string(datetime::now()));
			context.headers.emplace("MT-Reason", "fault");

			context.headers.emplace("MT-Host-MachineName", host_.machine_name);
			context.headers.emplace("MT-Host-ProcessName", host_.process_name);
			context.headers.emplace("MT-Host-ProcessId", std::to_string(host_.process_id));
			context.headers.emplace("MT-Host-Assembly", host_.assembly);
			context.headers.emplace("MT-Host-AssemblyVersion", host_.assembly_version);
			context.headers.emplace("MT-Host-MassTransitVersion", host_.masstransit_version);
			context.headers.emplace("MT-Host-FrameworkVersion", host_.framework_version);
			context.headers.emplace("MT-Host-OperatingSystemVersion", host_.operating_system_version);

			publish_endpoint_->publish(context, get_error_queue(queue_));
		}

		std::string receive_endpoint::get_error_queue(std::string const& queue)
		{
			return queue + "_error";
		}
	}
}
