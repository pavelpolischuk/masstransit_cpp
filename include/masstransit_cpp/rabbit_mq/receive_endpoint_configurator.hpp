#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint.hpp>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class MASSTRANSIT_CPP_API receive_endpoint_configurator : public i_receive_endpoint_configurator
		{
		public:
			receive_endpoint_configurator(amqp_host const& host, std::string const& queue_name);
			~receive_endpoint_configurator() override;

			receive_endpoint_configurator & auto_delete(bool is);
			receive_endpoint_configurator & exclusive(bool is);
			receive_endpoint_configurator & prefetch_count(uint16_t count);
			receive_endpoint_configurator & timeout(std::chrono::microseconds const& timeout);

			receive_endpoint::factory get_factory(std::shared_ptr<i_error_handler> const& error_handler) const;

		protected:
			bool auto_delete_{ false };
			bool exclusive_{ false };
			uint16_t prefetch_count_{ 1 };
			std::chrono::microseconds timeout_{ std::chrono::seconds(1) };

			amqp_host host_;

			static std::shared_ptr<receive_endpoint> build(const receive_endpoint_configurator& configurator, std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host, std::shared_ptr<i_error_handler> const& error_handler);
		};
	}
}
