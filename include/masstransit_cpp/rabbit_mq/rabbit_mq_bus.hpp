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
	namespace threads { class worker_thread; }
	namespace rabbit_mq { class amqp_channel; }

	struct MASSTRANSIT_CPP_API rabbit_mq_config
	{
		const amqp_host target_host;
		const host_info client_host;

		rabbit_mq_config(amqp_host const& target_host, host_info const& client_host);
	};

	class MASSTRANSIT_CPP_API rabbit_mq_bus : public bus, public std::enable_shared_from_this<rabbit_mq_bus>
	{
	public:
		~rabbit_mq_bus() override;

		void start() override;
		void wait() const override;
		void stop() override;

	protected:
		rabbit_mq_bus(rabbit_mq_config const& config,
			std::shared_ptr<rabbit_mq::exchange_manager> const& exchange_manager,
			std::shared_ptr<message_publisher> const& message_publisher,
			std::vector<rabbit_mq::receive_endpoint::factory> const& receivers_factories);
		
		std::future<bool> publish(consume_context_info message, std::string const& exchange) const override;

	friend class rabbit_mq_configurator;

	private:
		std::shared_ptr<rabbit_mq::amqp_channel> queue_channel_;
		std::vector<std::shared_ptr<rabbit_mq::receive_endpoint>> receivers_;
		
		const std::shared_ptr<rabbit_mq::exchange_manager> exchange_manager_;
		const std::vector<rabbit_mq::receive_endpoint::factory> receivers_factories_;
		const std::shared_ptr<message_publisher> message_publisher_;
		const rabbit_mq_config config_;

		std::unique_ptr<threads::worker_thread> publish_worker_;
	};
}
