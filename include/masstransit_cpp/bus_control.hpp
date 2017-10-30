#pragma once

#include <masstransit_cpp/global.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API bus_control
	{
	public:
		virtual ~bus_control() = default;

		virtual void start() = 0;
		virtual void stop() = 0;
	};
}