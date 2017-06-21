#include <masstransit_cpp/json.hpp>

namespace masstransit_cpp_tests
{
	struct message_mock
	{
		int id;

		message_mock(nlohmann::json const& obj);
		explicit message_mock(int id);
		
		static std::string message_type();

		nlohmann::json to_json() const;
	};
}
