#pragma once
#include <masstransit_cpp/global.hpp>

#include <set>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT exchange_manager
	{
	public:
		inline bool has_exchange(std::string const& type) const;

		void declare_message_type(std::string const& type, boost::shared_ptr<AmqpClient::Channel> const& channel);

	protected:
		std::set<std::string> exchanges_;
	};
}
