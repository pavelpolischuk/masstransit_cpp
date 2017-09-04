#include "masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"
#include <masstransit_cpp/threads/task_repeat.hpp>
#include <masstransit_cpp/threads/worker_thread.hpp>

#include <SimpleAmqpClient/Channel.h>
#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	rabbit_mq_bus::rabbit_mq_bus(amqp_host const& target_host, host_info const& client_info,
		std::shared_ptr<rabbit_mq::exchange_manager> const& exchange_manager,
		std::vector<rabbit_mq::receive_endpoint::builder> const& receivers_builders)
		: exchange_manager_(exchange_manager)
		, receivers_builders_(receivers_builders)
		, target_host_(target_host)
		, client_info_(client_info)
	{
		queue_channel_ = AmqpClient::Channel::CreateFromUri(target_host_.to_string());
	}

	rabbit_mq_bus::~rabbit_mq_bus()
	{
	}

	void rabbit_mq_bus::start()
	{
		for (auto const& b : receivers_builders_)
		{
			auto receiver = b();
			receiver->bind_queues(exchange_manager_);
			receivers_.push_back(receiver);
		}

		receiving_loop_ = std::make_unique<threads::task_repeat>(std::chrono::seconds(15), &rabbit_mq_bus::process_input_messages, this);
		publish_worker_ = std::make_unique<threads::worker_thread>();
	}

	void rabbit_mq_bus::stop()
	{
		receiving_loop_ = nullptr;
		publish_worker_ = nullptr;
		receivers_.clear();
	}

	std::future<bool> rabbit_mq_bus::publish_impl(consume_context_info const& m, std::string const& t) const
	{
		return publish_worker_->enqueue([this](consume_context_info const& message, std::string const& type) -> bool {
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
				return true;
			}
			catch (std::exception & ex)
			{
				BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: " << ex.what();
				return false;
			}
			catch (...)
			{
				BOOST_LOG_TRIVIAL(error) << "rabbit_mq_bus::publish_impl\n\tException: unknown";
				return false;
			}
		}, m, t);
	}

	bool rabbit_mq_bus::process_input_messages()
	{
		auto smth_consumed = false;
		for (auto & q : receivers_)
		{
			if (q->try_consume())
				smth_consumed = true;
		}

		return smth_consumed;
	}
}