#include "masstransit_cpp/in_memory/in_memory_bus.hpp"
#include "masstransit_cpp/utils/i_error_handler.hpp"

#include <future>

namespace masstransit_cpp
{
	in_memory_bus::in_memory_bus(std::vector<in_memory::receive_endpoint::factory> const& receivers_factories, 
		std::shared_ptr<i_error_handler> error_handler)
		: receivers_factories_(receivers_factories)
		, error_handler_(std::move(error_handler))
	{
	}

	in_memory_bus::~in_memory_bus() = default;

	void in_memory_bus::start()
	{
		publish_worker_ = std::make_shared<threads::worker_thread>();
	
		const auto this_ptr = shared_from_this();
		for (auto const& factory : receivers_factories_)
		{
			receivers_.push_back(factory(this_ptr));
		}
	}

	void in_memory_bus::wait() const
	{
		const auto worker = publish_worker_;
		if (worker != nullptr)
			worker->wait();
	}

	void in_memory_bus::stop()
	{
		receivers_.clear();
		publish_worker_ = nullptr;
	}

	std::future<bool> in_memory_bus::publish(consume_context_info m, std::string const& _) const
	{
		return publish_worker_->enqueue([this](consume_context_info const& message) -> bool {
			try
			{
				for (auto const& receiver : receivers_)
					receiver->deliver(message);

				return true;
			}
			catch (std::exception & ex)
			{
				error_handler_->on_error("in_memory_bus::publish_impl", ex.what());
				return false;
			}
			catch (...)
			{
				error_handler_->on_error("in_memory_bus::publish_impl", "");
				return false;
			}
		}, m);
	}
}
