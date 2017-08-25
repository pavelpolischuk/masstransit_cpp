#include "masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"

#include <SimpleAmqpClient/Channel.h>
#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	rabbit_mq_bus::rabbit_mq_bus(amqp_host const& target_host, host_info const& client_info,
		std::shared_ptr<exchange_manager> const& exchange_manager, std::vector<std::shared_ptr<rabbit_mq::receive_endpoint>> const& receivers)
		: exchange_manager_(exchange_manager)
		, receivers_(receivers)
		, target_host_(target_host)
		, client_info_(client_info)
	{
		queue_channel_ = AmqpClient::Channel::CreateFromUri(target_host_.to_string());
	}

	rabbit_mq_bus::~rabbit_mq_bus()
	{
	}

	void rabbit_mq_bus::run()
	{
		while (working)
		{
			auto smth_consumed = false;
			for (auto & q : receivers_)
			{
				if (q->try_consume())
					smth_consumed = true;
			}

			if (!smth_consumed && working)
				std::this_thread::sleep_for(std::chrono::seconds(15));
		}
	}

	void rabbit_mq_bus::publish_impl(consume_context_info const& message, std::string const& type) const
	{
		auto for_send = message;
		for_send.send_host = client_info_;
		if (!exchange_manager_->has_exchange(type))
		{
			exchange_manager_->declare_message_type(type, queue_channel_);
		}

		auto body = for_send.to_json().dump(2);

		try
		{
			BOOST_LOG_TRIVIAL(debug) << "bus publish message:\n" << body;

			queue_channel_->BasicPublish(type, "", AmqpClient::BasicMessage::Create(body));

			BOOST_LOG_TRIVIAL(debug) << "[DONE]";
		}
		catch (std::exception & ex)
		{
			BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: " << ex.what();
		}
		catch (...)
		{
			BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: unknown";
		}
	}
}