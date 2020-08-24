#pragma once

#include <masstransit_cpp/global.hpp>

#include <string>

namespace masstransit_cpp
{
	struct MASSTRANSIT_CPP_API ssl_config
	{
		std::string path_to_ca_cert;
		std::string path_to_client_key;
		std::string path_to_client_cert;
		bool verify_hostname{false};

		friend bool MASSTRANSIT_CPP_API operator==(ssl_config const& lhs, ssl_config const& rhs);
		friend bool MASSTRANSIT_CPP_API operator!=(ssl_config const& lhs, ssl_config const& rhs);
	};
}
