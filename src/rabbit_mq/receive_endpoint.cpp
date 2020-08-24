#include "masstransit_cpp/rabbit_mq/receive_endpoint.hpp"
#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"
#include "masstransit_cpp/utils/datetime.hpp"

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		receive_endpoint::receive_endpoint(std::shared_ptr<amqp_channel> const& channel, 
			receive_endpoint_config const& config, consumers_map const& consumers_by_type,
			std::shared_ptr<i_publish_endpoint> const& publish_endpoint,
			std::shared_ptr<i_error_handler> const& error_handler)
			: i_receive_endpoint(consumers_by_type, publish_endpoint, error_handler)
			, config_(config)
			, worker_pool_(new threads::worker_pool(config.concurrency_limit))
			, channel_(channel)
		{
		}

		receive_endpoint::~receive_endpoint()
		{
			stop();
		}

		void receive_endpoint::start_listen()
		{
			receiving_loop_ = std::make_shared<threads::task_repeat>(
				std::chrono::seconds(1),
				&receive_endpoint::try_consume, this);	
		}

		bool receive_endpoint::try_consume() const
		{
			const auto worker = worker_pool_->get_free_worker();
			if(worker == nullptr) return false;

			std::shared_ptr<amqp_envelope> envelope;
			if (!channel_->consume_message(tag_, envelope, config_.timeout))
			{
				worker_pool_->return_worker(worker);
				return false;
			}

			if (envelope == nullptr)
			{
				worker_pool_->return_worker(worker);
				return false;
			}

			const auto message = envelope->message();
			auto body = message->body();
			auto body_j = nlohmann::json::parse(body.begin(), body.end());
			
			channel_->ack(envelope->get_delivery_info());

			const auto context = body_j.get<consume_context_info>();
			worker->enqueue([this, context]()
			{
				deliver_impl(context);
			});

			return true;
		}

		void receive_endpoint::bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager)
		{
			const auto error_queue = get_error_queue(config_.queue);

			exchange_manager->declare_exchange(config_.queue, channel_);
			exchange_manager->declare_exchange(error_queue, channel_);
			
			for (auto const& c : consumers_by_type_)
			{
				const auto type = exchange_manager->get_name_by_message_type(c.second->message_type());
				exchange_manager->declare_exchange(type, channel_);
				channel_->bind_exchange(config_.queue, type, "");
			}

			channel_->bind_queue(error_queue, error_queue);
			channel_->bind_queue(config_.queue, config_.queue);

			tag_ = channel_->consume(config_.queue, "", true, false, config_.exclusive, config_.prefetch_count);
		}

		void receive_endpoint::on_error(consume_context_info const& context, std::string const& consumer_type, std::exception const& ex) const
		{
			i_receive_endpoint::on_error(context, consumer_type, ex);

			auto new_context = context;  // NOLINT(performance-unnecessary-copy-initialization)
			new_context.headers.emplace("MT-Fault-Message", std::string(ex.what()));
			new_context.headers.emplace("MT-Fault-Timestamp", datetime::now().to_string());
			new_context.headers.emplace("MT-Reason", "fault");
			
			new_context.headers.emplace("MT-Host-MachineName", config_.host.machine_name);
			new_context.headers.emplace("MT-Host-ProcessName", config_.host.process_name);
			new_context.headers.emplace("MT-Host-ProcessId", std::to_string(config_.host.process_id));
			new_context.headers.emplace("MT-Host-Assembly", config_.host.assembly);
			new_context.headers.emplace("MT-Host-AssemblyVersion", config_.host.assembly_version);
			new_context.headers.emplace("MT-Host-MassTransitVersion", config_.host.masstransit_version);
			new_context.headers.emplace("MT-Host-FrameworkVersion", config_.host.framework_version);
			new_context.headers.emplace("MT-Host-OperatingSystemVersion", config_.host.operating_system_version);

			publish_endpoint_->publish(new_context, get_error_queue(config_.queue));
		}

		std::string receive_endpoint::get_error_queue(std::string const& queue)
		{
			return queue + "_error";
		}

		void receive_endpoint::wait() const
		{
			const auto loop = receiving_loop_;
			if(loop != nullptr)
				loop->wait();
		}

		void receive_endpoint::stop()
		{
			receiving_loop_.reset();
		}
	}
}
