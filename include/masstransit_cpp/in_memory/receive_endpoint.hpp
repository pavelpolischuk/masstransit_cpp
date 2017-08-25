#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/message_consumer.hpp>

#include <vector>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class MASSTRANSIT_CPP_EXPORT receive_endpoint
		{
		public:
			explicit receive_endpoint(std::map<std::string, std::shared_ptr<i_message_consumer>> const& consumers_by_type);

			bool deliver(consume_context_info const& message) const;

		private:
			std::map<std::string, std::shared_ptr<i_message_consumer>> consumers_by_type_;
			std::string tag_;

			std::shared_ptr<i_message_consumer> find_consumer(std::vector<std::string> const& message_types) const;
		};
	}
}