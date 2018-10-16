#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_message_consumer.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>

#include <vector>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API i_receive_endpoint
	{
	public:
		using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;

		i_receive_endpoint(consumers_map const& consumers_by_type, std::shared_ptr<i_publish_endpoint> const& publish_endpoint);

	protected:
		consumers_map consumers_by_type_;
		std::shared_ptr<i_publish_endpoint> publish_endpoint_;

		std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
	};
}
