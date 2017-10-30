#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>
#include <masstransit_cpp/threads/worker_thread.hpp>

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class exchange_manager;
		
		class MASSTRANSIT_CPP_API receive_endpoint : public i_receive_endpoint
		{
		public:
			using factory = std::function<std::shared_ptr<receive_endpoint>()>;
			
			explicit receive_endpoint(std::string const& queue, consumers_map const& consumers_by_type);

			template<class message_t>
			void deliver(consume_context<message_t> const& context)
			{
				consumer_worker_.enqueue([this](consume_context<message_t> const& message) {
					auto message_context = message;
					auto consumer = std::static_pointer_cast<message_consumer<message_t>>(find_consumer(message_context.message_types));
					if (consumer == nullptr)
						return;

					try
					{
						consumer->consume(message_context);
					}
					catch (std::exception & ex)
					{
						BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
							<< nlohmann::json(message_context.message).dump(2) << "\n\tException: " << ex.what();
					}
					catch (...)
					{
						BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
							<< nlohmann::json(message_context.message).dump(2) << "\n\tException: unknown";
					}
				}, context);
			}

			void deliver(consume_context_info const& context);

		private:
			const std::string queue_;
			threads::worker_thread consumer_worker_;
		};
	}
}
