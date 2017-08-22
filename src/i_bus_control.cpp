#include <masstransit_cpp/i_bus_control.hpp>

namespace masstransit_cpp
{
	i_bus_control::~i_bus_control()
	{
		stop();
	}

	void i_bus_control::start()
	{
		working = true;
		
		setup();
		
		thread_ = std::make_unique<std::thread>(&i_bus_control::run, this);
	}

	void i_bus_control::stop()
	{
		working = false;

		if (thread_ == nullptr) return;

		if (thread_->joinable()) thread_->join();

		thread_ = nullptr;
	}
}