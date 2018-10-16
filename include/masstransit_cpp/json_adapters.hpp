#pragma once

#include <boost/uuid/uuid.hpp>
#include <masstransit_cpp/json.hpp>

namespace boost
{
	namespace uuids
	{
		void to_json(nlohmann::json& j, boost::uuids::uuid const& p);
		void from_json(nlohmann::json const& j, boost::uuids::uuid & p);
	}
}
