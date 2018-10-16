#include "masstransit_cpp/i_receive_endpoint.hpp"

namespace masstransit_cpp
{
	i_receive_endpoint::i_receive_endpoint(consumers_map const& consumers_by_type, std::shared_ptr<i_publish_endpoint> const& publish_endpoint)
		: consumers_by_type_(consumers_by_type)
		, publish_endpoint_(publish_endpoint)
	{
	}

	std::shared_ptr<i_message_consumer> i_receive_endpoint::find_consumer(std::vector<std::string> const& message_types) const
	{
		for (auto const& message_type : message_types)
		{
			const auto consumer = consumers_by_type_.find(message_type);
			if (consumer != consumers_by_type_.end())
				return consumer->second;
		}

		return nullptr;
	}
}
