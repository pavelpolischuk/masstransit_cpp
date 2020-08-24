#include "masstransit_cpp/rabbit_mq/ssl_config.hpp"

namespace masstransit_cpp
{
	bool operator==(ssl_config const& lhs, ssl_config const& rhs)
	{
		return lhs.path_to_ca_cert == rhs.path_to_ca_cert
			&& lhs.path_to_client_key == rhs.path_to_client_key
			&& lhs.path_to_client_cert == rhs.path_to_client_cert
			&& lhs.verify_hostname == rhs.verify_hostname;
	}

	bool operator!=(ssl_config const& lhs, ssl_config const& rhs)
	{
		return !(lhs == rhs);
	}
}
