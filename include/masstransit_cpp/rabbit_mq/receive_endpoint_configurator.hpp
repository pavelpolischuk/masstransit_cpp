#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_receive_endpoint_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint.hpp>

#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <memory>

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
			receive_endpoint_configurator & prefetch_count(uint16_t count);
			receive_endpoint_configurator & poll_timeout(boost::posix_time::time_duration const& timeout);

			receive_endpoint::factory get_factory() const;

		protected:
			bool auto_delete_{ false };
			uint16_t prefetch_count_{ 1 };
			boost::posix_time::time_duration timeout_{ boost::posix_time::milliseconds(500) };

			amqp_host host_;

			static std::shared_ptr<receive_endpoint> build(receive_endpoint_configurator configuration, std::shared_ptr<i_publish_endpoint> const& publish_endpoint, host_info const& client_host);
		};
	}
}
