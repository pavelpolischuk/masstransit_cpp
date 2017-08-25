#include "catch.hpp"

#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>

namespace masstransit_cpp_tests
{
	using namespace masstransit_cpp;

	TEST_CASE("amqp_host_tests", "[amqp_host]")
	{
		SECTION("uri_to_string")
		{
			amqp_uri uri("192.16.1.1");
			REQUIRE(uri.to_string() == "amqp://192.16.1.1");
		}

		SECTION("local_uri")
		{
			REQUIRE(amqp_uri::localhost.host == "127.0.0.1");
		}

		SECTION("host_to_string")
		{
			amqp_uri uri("192.16.128.1");
			amqp_host host(uri);
			
			REQUIRE(host.to_string() == "amqp://192.16.128.1");
		}

		SECTION("host_with_user_to_string")
		{
			amqp_uri uri("192.16.128.1");
			amqp_host host(uri, "user");
			
			REQUIRE(host.to_string() == "amqp://user@192.16.128.1");
		}

		SECTION("host_with_auth_to_string")
		{
			amqp_uri uri("192.16.128.1");
			amqp_host host(uri, "user", "password");
			
			REQUIRE(host.to_string() == "amqp://user:password@192.16.128.1");
		}

		SECTION("host_configurator::get_host")
		{
			amqp_uri uri("192.16.128.1");
			amqp_host_configurator configurator(uri);
			configurator.username("user");
			configurator.password("password");

			amqp_host host(uri, "user", "password");
			
			REQUIRE(configurator.get_host() == host);
		}
	}
}