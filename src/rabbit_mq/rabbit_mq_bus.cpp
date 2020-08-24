#include "masstransit_cpp/rabbit_mq/rabbit_mq_bus.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"
#include "masstransit_cpp/threads/task_repeat.hpp"
#include "masstransit_cpp/threads/worker_thread.hpp"

namespace masstransit_cpp
{
	rabbit_mq_bus::rabbit_mq_bus(rabbit_mq_config const& config,
		std::shared_ptr<rabbit_mq::exchange_manager> const& exchange_manager,
		std::shared_ptr<message_publisher> const& message_publisher,
		std::vector<rabbit_mq::receive_endpoint::factory> const& receivers_factories)
		: queue_channel_(new rabbit_mq::amqp_channel(config.target_host))
		, exchange_manager_(exchange_manager)
		, receivers_factories_(receivers_factories)
		, message_publisher_(message_publisher)
		, config_(config)
	{
	}

	rabbit_mq_config::rabbit_mq_config(amqp_host const& target_host, host_info const& client_host)
		: target_host(target_host)
		, client_host(client_host)
	{
	}

	rabbit_mq_bus::~rabbit_mq_bus() = default;

	void rabbit_mq_bus::start()
	{
		publish_worker_ = std::make_unique<threads::worker_thread>();
	
		const auto this_ptr = shared_from_this();
		for (auto const& b : receivers_factories_)
		{
			auto receiver = b(this_ptr, config_.client_host);
			receiver->bind_queues(exchange_manager_);
			receiver->start_listen();
			receivers_.push_back(receiver);
		}
	}

	void rabbit_mq_bus::wait() const
	{
		for(auto const& receiver : receivers_)
			receiver->wait();
	}
	
	void rabbit_mq_bus::stop()
	{
		receivers_.clear();
		publish_worker_ = nullptr;
	}

	std::future<bool> rabbit_mq_bus::publish(consume_context_info message, std::string const& exchange) const
	{
		const auto prepared_exchange = exchange_manager_->get_name_by_message_type(exchange);	

		message.send_host = config_.client_host;
		return publish_worker_->enqueue([this](consume_context_info const& m, std::string const& e) -> bool
		{
			exchange_manager_->declare_exchange(e, queue_channel_);
			return message_publisher_->publish(m, queue_channel_, e);
		}, std::move(message), prepared_exchange);
	}
}
