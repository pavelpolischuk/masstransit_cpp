#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/i_bus.hpp>

#include <set>
#include <thread>

namespace masstransit_cpp
{
	struct uri;
	class exchange_manager;
	class receive_endpoint;
	class send_endpoint;

	class MASSTRANSIT_CPP_EXPORT bus : public i_bus
	{
	public:
		bus();
		~bus() override;
		
		bus & host(uri const& uri, std::function<void(send_endpoint&)> const& host_configurator);
		bus & receive_endpoint(uri const& uri, std::string const& queue, std::function<void(receive_endpoint&)> const& configurator);

		void start();
		void stop();

		std::set<std::string> const& exchanges() const;

	protected:
		std::shared_ptr<std::thread> thread_;
		std::shared_ptr<exchange_manager> exchange_manager_;
		std::shared_ptr<send_endpoint> send_endpoint_;
		std::map<std::string, masstransit_cpp::receive_endpoint> receivers_;
		
		volatile bool working = false;

		void setup();
		void publish_impl(consume_context_info const& message, std::string const& type) const override;
	};
}
