#include "masstransit_cpp/utils/console_error_handler.hpp"
#include "masstransit_cpp/utils/datetime.hpp"

#include <iostream>

namespace masstransit_cpp
{
	void i_error_handler::on_error(std::string const& method, std::string const& message)
	{
	}
	
	void console_error_handler::on_error(std::string const& method, std::string const& message)
	{
		std::cout << "ERROR <" << datetime::now() << "> " << method << ": " << message << std::endl;
	}
}
