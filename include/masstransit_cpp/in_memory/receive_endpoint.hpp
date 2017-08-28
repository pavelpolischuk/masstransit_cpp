#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class MASSTRANSIT_CPP_EXPORT receive_endpoint : public i_receive_endpoint
		{
		public:
			explicit receive_endpoint(consumers_map const& consumers_by_type);

			bool deliver(consume_context_info const& message) const;

		private:
		};
	}
}