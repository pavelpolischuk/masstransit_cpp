#pragma once

#include <masstransit_cpp/global.hpp>

#include <thread>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT bus_control
	{
	public:
		virtual ~bus_control();

		void start();
		void stop();

	protected:
		std::unique_ptr<std::thread> thread_;
		volatile bool working = false;

		virtual void run() = 0;
	};
}