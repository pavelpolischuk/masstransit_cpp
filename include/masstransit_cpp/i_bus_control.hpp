#pragma once

#include <masstransit_cpp/global.hpp>

#include <thread>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT i_bus_control
	{
	public:
		virtual ~i_bus_control();

		void start();
		void stop();

	protected:
		std::unique_ptr<std::thread> thread_;
		volatile bool working = false;

		virtual void setup() = 0;
		virtual void run() = 0;
	};
}
