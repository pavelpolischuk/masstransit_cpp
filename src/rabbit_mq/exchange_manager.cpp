#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"

#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		exchange_manager::exchange_manager(const bool auto_delete)
			: auto_delete_(auto_delete)
		{}

		bool exchange_manager::has_exchange(std::string const& type) const
		{
			return exchanges_.find(type) != exchanges_.end();
		}

		void exchange_manager::declare_exchange(std::string const& name, boost::shared_ptr<AmqpClient::Channel> const& channel)
		{
			if (has_exchange(name))
				return;
			
			try
			{
				channel->DeclareExchange(name, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT, false, true, auto_delete_);
				exchanges_.insert(name);
			}
			catch (std::exception const& ex)
			{
				BOOST_LOG_TRIVIAL(error) << "exchange_manager::declare_message_type\n\tException: " << ex.what();
			}
			catch (...)
			{
				BOOST_LOG_TRIVIAL(error) << "exchange_manager::declare_message_type\n\tException: unknown";
			}
		}
	}
}
