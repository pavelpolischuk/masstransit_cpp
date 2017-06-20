#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/exchange_manager.hpp>

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	bus::bus()
		: exchange_manager_(std::make_shared<exchange_manager>())
	{
	}

	bus::~bus()
	{
		stop();
	}

	bus& bus::host(uri const& uri, std::function<void(send_endpoint&)> const& host_configurator)
	{
		if (send_endpoint_ != nullptr)
			throw std::exception("host has been already configured.");

		send_endpoint_ = std::make_shared<send_endpoint>(uri);
		host_configurator(*send_endpoint_);
		return *this;
	}

	bus& bus::receive_endpoint(uri const& uri, std::string const& queue, std::function<void(masstransit_cpp::receive_endpoint&)> const& configurator)
	{
		auto key = uri.host + queue;
		auto q = receivers_.find(key);
		if (q != receivers_.end())
		{
			configurator(q->second);
			return *this;
		}

		auto inserted = receivers_.insert({ key, { uri, queue, boost::posix_time::milliseconds(300) } });
		if(inserted.second)
		{
			configurator(inserted.first->second);
		}

		return *this;
	}

	void bus::start()
	{
		working = true;
		
		setup();

		thread_ = std::make_shared<std::thread>([this]()
		{
			while(working)
			{
				auto smth_consumed = false;
				for (auto & q : receivers_)
				{
					if (q.second.try_consume())
						smth_consumed = true;
				}

				if (!smth_consumed)
					std::this_thread::sleep_for(std::chrono::seconds(15));
			}
		});
	}

	void bus::stop()
	{
		working = false;

		if (thread_ == nullptr) return;

		if (thread_->joinable()) thread_->join();

		thread_ = nullptr;
	}

	void bus::setup()
	{
		if (send_endpoint_ == nullptr)
			BOOST_LOG_TRIVIAL(warning) << "bus host (send_endpoint) has not been configured, publish message is impossible.";
		else
			send_endpoint_->connect(exchange_manager_);

		for (auto & q : receivers_)
			q.second.connect(exchange_manager_);
	}

	void bus::publish_impl(consume_context_info const& message, std::string const& type) const
	{
		if (send_endpoint_ == nullptr)
			throw std::exception("host has not been configured.");

		send_endpoint_->send(message, type);
	}
}