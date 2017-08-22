#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/uri.hpp>
#include <masstransit_cpp/message_consumer.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	class exchange_manager;

	class MASSTRANSIT_CPP_EXPORT receive_endpoint
	{
	public:
		receive_endpoint(uri const& uri, std::string const& name);

		template<class message_t>
		receive_endpoint & consumer(std::shared_ptr<message_consumer<message_t>> const& consumer)
		{
			consumers_by_type_["urn:message:" + message_t::message_type()] = consumer;
			return *this;
		}

		receive_endpoint & poll_timeout(boost::posix_time::time_duration const& timeout);
		receive_endpoint & auto_delete(bool is);
		receive_endpoint & prefetch_count(uint16_t count);

	private:
		boost::shared_ptr<AmqpClient::Channel> queue_channel_;
		std::map<std::string, std::shared_ptr<i_message_consumer>> consumers_by_type_;
		std::string tag_;
		bool auto_delete_{ false };
		uint16_t prefetch_count_{ 1 };

		int timeout_{ 500 };
		std::string queue_;
		uri uri_;
		
		void connect(std::shared_ptr<exchange_manager> const& exchange_manager);
		std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
		bool try_consume() const;
	
		friend class bus;
	};
}