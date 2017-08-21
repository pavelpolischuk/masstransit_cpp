#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/host_info.hpp>
#include <masstransit_cpp/uri.hpp>

#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	struct consume_context_info;
	class exchange_manager;

	class MASSTRANSIT_CPP_EXPORT send_endpoint
	{
	public:
		explicit send_endpoint(uri const& uri);

		send_endpoint & host(host_info const& host);

	private:
		boost::shared_ptr<AmqpClient::Channel> queue_channel_;
		std::shared_ptr<exchange_manager> exchange_manager_;
		host_info host_;
		uri uri_;
		
		void send(consume_context_info const& message, std::string const& type) const;
		void connect(std::shared_ptr<exchange_manager> const& exchange_manager);

		friend class bus;
	};
}