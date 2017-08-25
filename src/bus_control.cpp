#include "masstransit_cpp/bus_control.hpp"

namespace masstransit_cpp
{
	bus_control::~bus_control()
	{
		stop();
	}

	void bus_control::start()
	{
		working = true;
		
		thread_ = std::make_unique<std::thread>(&bus_control::run, this);
	}

	void bus_control::stop()
	{
		working = false;

		if (thread_ == nullptr) return;
		if (thread_->joinable()) thread_->join();

		thread_ = nullptr;
	}
}