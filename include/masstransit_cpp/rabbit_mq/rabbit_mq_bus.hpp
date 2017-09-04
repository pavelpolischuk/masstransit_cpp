#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/host_info.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/exchange_manager.hpp>

namespace masstransit_cpp
{
	namespace threads {
		class task_repeat;
		class worker_thread;
	}

	class MASSTRANSIT_CPP_EXPORT rabbit_mq_bus : public bus
	{
	public:
		rabbit_mq_bus(amqp_host const& target_host, host_info const& client_info, std::shared_ptr<rabbit_mq::exchange_manager> const& exchange_manager, std::vector<rabbit_mq::receive_endpoint::builder> const& receivers_builders);
		~rabbit_mq_bus() override;

		void start() override;
		void stop() override;

	protected:
		std::future<bool> publish_impl(consume_context_info const& message, std::string const& type) const override;

	private:
		boost::shared_ptr<AmqpClient::Channel> queue_channel_;
		std::shared_ptr<rabbit_mq::exchange_manager> exchange_manager_;
		std::vector<std::shared_ptr<rabbit_mq::receive_endpoint>> receivers_;
		std::vector<rabbit_mq::receive_endpoint::builder> receivers_builders_;
		amqp_host target_host_;
		host_info client_info_;

		std::unique_ptr<threads::task_repeat> receiving_loop_;
		std::unique_ptr<threads::worker_thread> publish_worker_;

		bool process_input_messages();
	};
}