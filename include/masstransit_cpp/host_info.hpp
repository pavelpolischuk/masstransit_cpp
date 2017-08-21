#pragma once

#include <masstransit_cpp/global.hpp>
#include <masstransit_cpp/json.hpp>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_EXPORT host_info
	{
		host_info();
		explicit host_info(nlohmann::json const& obj);

		std::string machine_name;
		std::string process_name;
		int64_t process_id{ 0 };
		std::string assembly;
		std::string assembly_version;
		std::string framework_version;
		std::string masstransit_version;
		std::string operating_system_version;

		nlohmann::json to_json() const;

		friend bool operator==(host_info const& lhv, host_info const& rhv);
	};
}