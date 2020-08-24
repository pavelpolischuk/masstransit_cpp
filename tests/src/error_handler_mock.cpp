#include "error_handler_mock.hpp"

namespace masstransit_cpp_tests
{
	error_handler_mock::error_handler_mock() = default;
	error_handler_mock::~error_handler_mock() = default;

	void error_handler_mock::on_error(std::string const& method, std::string const& message)
	{
		saved_message = message;
	}
}
