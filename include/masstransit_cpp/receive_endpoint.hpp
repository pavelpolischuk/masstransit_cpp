#pragma once
#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/uri.hpp>

#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

namespace masstransit_cpp
{
	class exchange_manager;
	class i_message_consumer;

	class MASSTRANSIT_CPP_EXPORT receive_endpoint
	{
	public:
		receive_endpoint(uri const& uri, std::string const& name);

		receive_endpoint & consumer(std::shared_ptr<i_message_consumer> const& consumer);
		receive_endpoint & poll_timeout(boost::posix_time::time_duration const& timeout);

	private:
		boost::shared_ptr<AmqpClient::Channel> queue_channel_;
		std::map<std::string, std::shared_ptr<i_message_consumer>> consumers_by_type_;
		std::string tag_;

		int timeout_{ 500 };
		std::string queue_;
		uri uri_;
		
		void connect(std::shared_ptr<exchange_manager> const& exchange_manager);
		std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
		bool try_consume() const;
	
		friend class bus;
	};
}
