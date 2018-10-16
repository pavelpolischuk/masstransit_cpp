#include <masstransit_cpp/json.hpp>

namespace masstransit_cpp_tests
{
	struct message_mock
	{
		int id;

		message_mock();
		explicit message_mock(int id);
		
		static std::string message_type();
	};

	void to_json(nlohmann::json& j, message_mock const& p);
	void from_json(nlohmann::json const& j, message_mock & p);
}
