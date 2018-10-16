#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>
#include <masstransit_cpp/message_consumer.hpp>
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
			using factory = std::function<std::shared_ptr<receive_endpoint>(std::shared_ptr<i_publish_endpoint> const& publish_endpoint)>;
			
			explicit receive_endpoint(std::string const& queue, 
				consumers_map const& consumers_by_type, 
				std::shared_ptr<i_publish_endpoint> const& publish_endpoint);

			template<class MessageT>
			void deliver(consume_context<MessageT> const& context)
			{
				consumer_worker_.enqueue([this](consume_context<MessageT> const& message) {
					auto message_context = message;
					auto consumer = std::static_pointer_cast<message_consumer<MessageT>>(find_consumer(message_context.message_types));
					if (consumer == nullptr)
						return;

					auto body = nlohmann::json(message_context.message).dump(2);
					try
					{
						BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;
						
						consumer->consume(message_context);

						BOOST_LOG_TRIVIAL(debug) << "[DONE]";
					}
					catch (std::exception & ex)
					{
						BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
							<< body << "\n\tException: " << ex.what();
					}
					catch (...)
					{
						BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
							<< body << "\n\tException: unknown";
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
