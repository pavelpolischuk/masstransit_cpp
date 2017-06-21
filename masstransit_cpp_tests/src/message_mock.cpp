#include <message_mock.hpp>

namespace masstransit_cpp_tests
{
	message_mock::message_mock(nlohmann::json const& obj)
		: id(obj.get<int>("id"))
	{}

	message_mock::message_mock(int id)
		: id(id)
	{}

	std::string message_mock::message_type()
	{
		return "Test.Domain.Contracts:HandlingRequest";
	}

	nlohmann::json message_mock::to_json() const
	{
		return{ {"id", id} };
	}
}