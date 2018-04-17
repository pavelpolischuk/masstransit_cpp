#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/json.hpp>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API host_info
	{
		host_info();

		std::string machine_name;
		std::string process_name;
		int64_t process_id{ 0 };
		std::string assembly;
		std::string assembly_version;
		std::string framework_version;
		std::string masstransit_version;
		std::string operating_system_version;

		friend bool operator==(host_info const& lhv, host_info const& rhv);
	};

	void MASSTRANSIT_CPP_API to_json(nlohmann::json& j, host_info const& p);
	void MASSTRANSIT_CPP_API from_json(nlohmann::json const& j, host_info & p);
}
