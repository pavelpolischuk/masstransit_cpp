#include <masstransit_cpp/rabbit_mq/exchange_manager.hpp>

#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	void exchange_manager::auto_delete(bool is)
	{
		auto_delete_ = is;
	}

	void exchange_manager::declare_message_type(std::string const& type, boost::shared_ptr<AmqpClient::Channel> const& channel)
	{
		if (exchanges_.count(type) == 0)
		{
			try
			{
				channel->DeclareExchange(type, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT, false, true, auto_delete_);
				exchanges_.insert(type);
			}
			catch (std::exception ex)
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