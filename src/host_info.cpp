#include <masstransit_cpp/host_info.hpp>

namespace masstransit_cpp
{
	host_info::host_info()
	{}

	void to_json(nlohmann::json& j, host_info const& p)
	{
		j = {
			{ "machineName", p.machine_name },
			{ "processName", p.process_name },
			{ "processId", p.process_id },
			{ "assembly", p.assembly },
			{ "assemblyVersion", p.assembly_version },
			{ "frameworkVersion", p.framework_version },
			{ "massTransitVersion", p.masstransit_version },
			{ "operatingSystemVersion", p.operating_system_version }
		};
	}

	void from_json(nlohmann::json const& j, host_info& p)
	{
		p.machine_name = j.at("machineName").get<std::string>();
		p.process_name = j.at("processName").get<std::string>();
		p.process_id = j.at("processId").get<int64_t>();
		p.assembly = j.at("assembly").get<std::string>();
		p.assembly_version = j.at("assemblyVersion").get<std::string>();
		p.framework_version = j.at("frameworkVersion").get<std::string>();
		p.masstransit_version = j.at("massTransitVersion").get<std::string>();
		p.operating_system_version = j.at("operatingSystemVersion").get<std::string>();
	}

	bool operator==(host_info const& lhv, host_info const& rhv)
	{
		return
			lhv.machine_name == rhv.machine_name &&
			lhv.process_name == rhv.process_name &&
			lhv.process_id == rhv.process_id &&
			lhv.assembly == rhv.assembly &&
			lhv.assembly_version == rhv.assembly_version &&
			lhv.framework_version == rhv.framework_version &&
			lhv.masstransit_version == rhv.masstransit_version &&
			lhv.operating_system_version == rhv.operating_system_version;
	}
}
