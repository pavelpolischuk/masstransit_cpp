#pragma once

#include <masstransit_cpp/utils/i_error_handler.hpp>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API console_error_handler : public i_error_handler
	{
	public:
		~console_error_handler() override = default;
		
		void on_error(std::string const& method, std::string const& message) override;
	};
}
