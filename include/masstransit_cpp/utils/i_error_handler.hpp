#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API i_error_handler
	{
	public:
		virtual ~i_error_handler() = default;

		virtual void on_error(std::string const& method, std::string const& message);
	};

}
