#pragma once

#include <masstransit_cpp/utils/i_error_handler.hpp>
#include <optional>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::i_error_handler;

	class error_handler_mock : public i_error_handler
	{
	public:
		std::optional<std::string> saved_message;

		error_handler_mock();
		~error_handler_mock() override;

		void on_error(std::string const& method, std::string const& message) override;
	};
}
