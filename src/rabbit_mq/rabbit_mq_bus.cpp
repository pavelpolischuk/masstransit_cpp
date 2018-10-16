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
		std::vector<rabbit_mq::receive_endpoint::factory> const& receivers_factories)
		: exchange_manager_(exchange_manager)
		, receivers_factories_(receivers_factories)
		, target_host_(target_host)
		, host_info_(client_info)
		, message_publisher_{}
	{
		queue_channel_ = target_host_.create_channel();
	}

	rabbit_mq_bus::~rabbit_mq_bus()
	{
	}

	void rabbit_mq_bus::start()
	{
		const auto this_ptr = shared_from_this();
		for (auto const& b : receivers_factories_)
		{
			auto receiver = b(this_ptr, host_info_);
			receiver->bind_queues(exchange_manager_);
			receivers_.push_back(receiver);
		}

		receiving_loop_ = std::make_shared<threads::task_repeat>(std::chrono::seconds(15), &rabbit_mq_bus::process_input_messages, this);
		publish_worker_ = std::make_unique<threads::worker_thread>();
	}

	void rabbit_mq_bus::wait() const
	{
		const auto loop = receiving_loop_;
		if(loop != nullptr)
			loop->wait();
	}
	
	void rabbit_mq_bus::stop()
	{
		receiving_loop_->stop();
		publish_worker_ = nullptr;
		receiving_loop_ = nullptr;
		receivers_.clear();
	}

	std::future<bool> rabbit_mq_bus::publish(consume_context_info const& m, std::string const& e) const
	{
		return publish_worker_->enqueue([this](consume_context_info const& message, std::string const& exchange) -> bool
		{
			exchange_manager_->declare_exchange(exchange, queue_channel_);
			return message_publisher_.publish(message, queue_channel_, exchange);
		}, m, e);
	}

	void rabbit_mq_bus::fill(consume_context_info & message, std::string const& exchange) const
	{
		message.send_host = host_info_;
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
