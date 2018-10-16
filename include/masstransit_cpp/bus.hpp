#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/bus_control.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API bus : public bus_control, public i_publish_endpoint
	{
	public:
		~bus() override = default;
	};
}
