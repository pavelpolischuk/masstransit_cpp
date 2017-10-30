#include "masstransit_cpp/in_memory/exchange_manager.hpp"

#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	namespace in_memory
	{
		exchange_manager::exchange_manager(bool auto_delete)
			: auto_delete_(auto_delete)
		{}

		bool exchange_manager::has_exchange(std::string const& type) const
		{
			return exchanges_.count(type) > 0;
		}

		void exchange_manager::declare_message_type(std::string const& type)
		{
			if (exchanges_.count(type) == 0)
			{
				try
				{
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
}