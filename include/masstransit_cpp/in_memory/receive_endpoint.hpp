#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class exchange_manager;
		
		class MASSTRANSIT_CPP_EXPORT receive_endpoint : public i_receive_endpoint
		{
		public:
			using builder = std::function<std::shared_ptr<receive_endpoint>()>;
			
			explicit receive_endpoint(std::string const& queue, consumers_map const& consumers_by_type);

			template<class message_t>
			void deliver(consume_context<message_t> const& context) const
			{
				auto consumer = std::static_pointer_cast<message_consumer<message_t>>(find_consumer(context.message_types));
				if (consumer == nullptr)
					return;

				try
				{
					consumer->consume(context);
				}
				catch (std::exception & ex)
				{
					BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
						<< context.message.to_json().dump(2) << "\n\tException: " << ex.what();
				}
				catch (...)
				{
					BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
						<< context.message.to_json().dump(2) << "\n\tException: unknown";
				}
			}

			void deliver(consume_context_info const& context) const;

			void bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager);

		private:
			const std::string queue_;
		};
	}
}
