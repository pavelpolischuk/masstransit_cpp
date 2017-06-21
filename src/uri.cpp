#include <masstransit_cpp/uri.hpp>

namespace masstransit_cpp
{
	uri::uri(std::string const& host, std::string const& user, std::string const& password)
		: host(host)
		, user(user)
		, password(password)
	{
	}

	std::string uri::to_string() const
	{
		if (user.empty()) return "amqp://" + host;
		if (password.empty()) return "amqp://" + user + "@" + host;
		return "amqp://" + user + ":" + password + "@" + host;
	}
}
