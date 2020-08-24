#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_config.hpp>
#include <masstransit_cpp/threads/worker_pool.hpp>
#include <masstransit_cpp/threads/task_repeat.hpp>

#include <memory>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_channel;
		class exchange_manager;

		class MASSTRANSIT_CPP_API receive_endpoint : public i_receive_endpoint
		{
		public:
			using factory = std::function<std::shared_ptr<receive_endpoint>(std::shared_ptr<i_publish_endpoint> const&, host_info const&)>;

			receive_endpoint(std::shared_ptr<amqp_channel> const& channel, 
				receive_endpoint_config const& config, consumers_map const& consumers_by_type,
				std::shared_ptr<i_publish_endpoint> const& publish_endpoint, 
				std::shared_ptr<i_error_handler> const& error_handler);

			~receive_endpoint() override;

			void start_listen();

			bool try_consume() const;
			void bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager);

			static std::string get_error_queue(std::string const& queue);

			void wait() const;
			void stop();

		private:
			const receive_endpoint_config config_;

			std::shared_ptr<threads::task_repeat> receiving_loop_;
			std::shared_ptr<threads::worker_pool> worker_pool_;
			std::shared_ptr<amqp_channel> channel_;
			std::string tag_;
			size_t tasks_in_process_{ 0 };

			void on_error(consume_context_info const& context, std::string const& consumer_type, std::exception const& ex) const override;
		};
	}
}
