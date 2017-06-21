#include <masstransit_cpp/host_info.hpp>

namespace masstransit_cpp
{
	host_info::host_info()
	{}

	host_info::host_info(nlohmann::json const& obj)
	{
		machine_name = obj.get<std::string>("machineName", "");
		process_name = obj.get<std::string>("processName", "");
		process_id = obj.get<int64_t>("processId", 0);
		assembly = obj.get<std::string>("assembly", "");
		assembly_version = obj.get<std::string>("assemblyVersion", "");
		framework_version = obj.get<std::string>("frameworkVersion", "");
		masstransit_version = obj.get<std::string>("massTransitVersion", "");
		operating_system_version = obj.get<std::string>("operatingSystemVersion", "");
	}

	nlohmann::json host_info::to_json() const
	{
		return{
			{ "machineName", machine_name },
			{ "processName", process_name },
			{ "processId", process_id },
			{ "assembly", assembly },
			{ "assemblyVersion", assembly_version },
			{ "frameworkVersion", framework_version },
			{ "massTransitVersion", masstransit_version },
			{ "operatingSystemVersion", operating_system_version }
		};
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
