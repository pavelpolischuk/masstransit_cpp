#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/host_info.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/exchange_manager.hpp>
#include <masstransit_cpp/rabbit_mq/message_publisher.hpp>

namespace masstransit_cpp
{
	namespace threads {
		class task_repeat;
		class worker_thread;
	}

	class MASSTRANSIT_CPP_API rabbit_mq_bus : public bus, public std::enable_shared_from_this<rabbit_mq_bus>
	{
	public:
		~rabbit_mq_bus() override;

		void start() override;
		void wait() const override;
		void stop() override;

	protected:
		rabbit_mq_bus(amqp_host const& target_host,
			host_info const& client_info,
			std::shared_ptr<rabbit_mq::exchange_manager> const& exchange_manager,
			std::vector<rabbit_mq::receive_endpoint::factory> const& receivers_factories);
		
		std::future<bool> publish(consume_context_info const& message, std::string const& exchange) const override;

		void fill(consume_context_info & message, std::string const& exchange) const override;

	friend class rabbit_mq_configurator;

	private:
		boost::shared_ptr<AmqpClient::Channel> queue_channel_;
		std::shared_ptr<rabbit_mq::exchange_manager> exchange_manager_;
		std::vector<std::shared_ptr<rabbit_mq::receive_endpoint>> receivers_;
		const std::vector<rabbit_mq::receive_endpoint::factory> receivers_factories_;
		const amqp_host target_host_;
		const host_info host_info_;
		const message_publisher message_publisher_;

		std::shared_ptr<threads::task_repeat> receiving_loop_;
		std::unique_ptr<threads::worker_thread> publish_worker_;

		bool process_input_messages();
	};
}
