#include "masstransit_cpp/utils/datetime.hpp"

#include <iostream>

namespace masstransit_cpp
{
	datetime::datetime(std::chrono::system_clock::time_point const& d)
		: std::chrono::system_clock::time_point(d)
	{
	}

	datetime datetime::now()
	{
		return datetime(std::chrono::system_clock::now());
	}
}
