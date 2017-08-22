#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_bus_control.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>

#include <set>
#include <thread>

namespace masstransit_cpp
{
	struct uri;
	class exchange_manager;
	class receive_endpoint;
	class send_endpoint;

	class MASSTRANSIT_CPP_EXPORT bus : public i_bus_control, public i_publish_endpoint
	{
	public:
		bus();
		~bus() override;
		
		bus & auto_delete(bool is);
		bus & host(uri const& uri, std::function<void(send_endpoint&)> const& host_configurator);
		bus & receive_endpoint(uri const& uri, std::string const& queue, std::function<void(receive_endpoint&)> const& configurator);

		std::set<std::string> const& exchanges() const;

	protected:
		std::unique_ptr<send_endpoint> send_endpoint_;
		std::shared_ptr<exchange_manager> exchange_manager_;
		std::map<std::string, masstransit_cpp::receive_endpoint> receivers_;

		void setup() override;
		void run() override;

		void publish_impl(consume_context_info const& message, std::string const& type) const override;
	};
}
