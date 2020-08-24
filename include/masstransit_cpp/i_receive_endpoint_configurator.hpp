#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_handler.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API i_receive_endpoint_configurator
	{
	public:
		explicit i_receive_endpoint_configurator(std::string const& queue_name);
		
		virtual ~i_receive_endpoint_configurator() = default;

		template<class MessageT>
		i_receive_endpoint_configurator & consumer(std::function<std::shared_ptr<message_consumer<MessageT>>()> const& consumer_factory)
		{
			add_consumer_factory(MessageT::message_type(), consumer_factory);
			return *this;
		}

		template<class MessageT>
		i_receive_endpoint_configurator & handler(std::function<void(consume_context<MessageT> const&)> const& handler)
		{
			return this->consumer<MessageT>(std::make_shared<message_handler<MessageT>>(handler));
		}

		template<class MessageT, class BoostDiContainerT>
		i_receive_endpoint_configurator & load_from(BoostDiContainerT const& container)
		{
			return this->consumer<MessageT>([&container]() -> std::shared_ptr<message_consumer<MessageT>>
			{
				return container.template create<std::shared_ptr<message_consumer<MessageT>>>();
			});
		}

		template<class MessageT>
		i_receive_endpoint_configurator & consumer(std::shared_ptr<message_consumer<MessageT>> const& consumer_instance)
		{
			return this->consumer<MessageT>([consumer_instance]() -> std::shared_ptr<message_consumer<MessageT>> 
			{ 
				return consumer_instance; 
			});
		}

		void use_concurrency_limit(size_t limit);

	protected:
		using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;
		using consumers_factories_map = std::map<std::string, std::function<std::shared_ptr<i_message_consumer>()>>;

		consumers_factories_map consumers_factories_by_type_;
		std::string queue_;
		size_t concurrency_limit_ = 1;

		virtual void add_consumer_factory(std::string const& message_type, std::function<std::shared_ptr<i_message_consumer>()> const& factory);
		consumers_map create_consumers() const;
	};
}
