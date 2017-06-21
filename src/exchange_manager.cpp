#include <masstransit_cpp/exchange_manager.hpp>

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	void exchange_manager::declare_message_type(std::string const& type, boost::shared_ptr<AmqpClient::Channel> const& channel)
	{
		if (exchanges_.count(type) == 0)
		{
			try
			{
				channel->DeclareExchange(type, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT, false, true, false);
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