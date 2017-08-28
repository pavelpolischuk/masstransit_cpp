#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_consumer.hpp>

#include <vector>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT i_receive_endpoint
	{
	public:
		using consumers_map = std::map<std::string, std::shared_ptr<i_message_consumer>>;

		explicit i_receive_endpoint(consumers_map const& consumers_by_type);

	protected:
		consumers_map consumers_by_type_;

		std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
	};
}