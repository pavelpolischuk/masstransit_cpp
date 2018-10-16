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

		template<class message_t>
		i_receive_endpoint_configurator & consumer(std::function<std::shared_ptr<message_consumer<message_t>>()> const& consumer_factory)
		{
			add_consumer_factory(message_t::message_type(), consumer_factory);
			return *this;
		}

		template<class message_t>
		i_receive_endpoint_configurator & handler(std::function<void(consume_context<message_t> const&)> const& handler)
		{
			return this->consumer<message_t>(std::make_shared<message_handler<message_t>>(handler));
		}

		template<class message_t, class boost_di_container_t>
		i_receive_endpoint_configurator & load_from(boost_di_container_t const& container)
		{
			return this->consumer<message_t>([&container]() -> std::shared_ptr<message_consumer<message_t>>
			{
				return container.template create<std::shared_ptr<message_consumer<message_t>>>();
			});
		}

		template<class message_t>
		i_receive_endpoint_configurator & consumer(std::shared_ptr<message_consumer<message_t>> const& consumer_instance)
		{
			return this->consumer<message_t>([consumer_instance]() -> std::shared_ptr<message_consumer<message_t>> 
			{ 
				return consumer_instance; 
			});
		}

	protected:
		using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;
		using consumers_factories_map = std::map<std::string, std::function<std::shared_ptr<i_message_consumer>()>>;

		consumers_factories_map consumers_factories_by_type_;
		std::string queue_;

		virtual void add_consumer_factory(std::string const& message_type, std::function<std::shared_ptr<i_message_consumer>()> const& factory);
		consumers_map create_consumers() const;
	};
}
