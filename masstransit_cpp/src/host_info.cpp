#include <masstransit_cpp/host_info.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

namespace masstransit_cpp
{
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
}
