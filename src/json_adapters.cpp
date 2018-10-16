#include <masstransit_cpp/json_adapters.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>

void boost::uuids::to_json(nlohmann::json& j, boost::uuids::uuid const& p)
{
	j = to_string(p);
}

void boost::uuids::from_json(nlohmann::json const& j, boost::uuids::uuid& p)
{
	p = boost::uuids::string_generator()(j.get<std::string>());
}
