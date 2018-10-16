#include "message_mock.hpp"

namespace masstransit_cpp_tests
{
	message_mock::message_mock()
		: id(0)
	{
	}

	message_mock::message_mock(int id)
		: id(id)
	{}

	std::string message_mock::message_type()
	{
		return "Test.Domain.Contracts:HandlingRequest";
	}

	void to_json(nlohmann::json& j, message_mock const& p)
	{
		j = { { "id", p.id } };
	}

	void from_json(nlohmann::json const& j, message_mock& p)
	{
		p.id = j.at("id").get<int>();
	}
}
