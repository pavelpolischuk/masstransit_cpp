#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class exchange_manager;
		
		class MASSTRANSIT_CPP_EXPORT receive_endpoint : public i_receive_endpoint
		{
		public:
			using builder = std::function<std::shared_ptr<receive_endpoint>()>;
			
			explicit receive_endpoint(std::string const& queue, consumers_map const& consumers_by_type);

			bool deliver(consume_context_info const& message) const;
			void bind_queues(std::shared_ptr<exchange_manager> const& exchange_manager);

		private:
			const std::string queue_;
		};
	}
}
