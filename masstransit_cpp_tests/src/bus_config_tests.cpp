#include "message_consumer_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/uri.hpp>
#include <masstransit_cpp/send_endpoint.hpp>
#include <masstransit_cpp/receive_endpoint.hpp>

namespace masstransit_cpp_tests
{
	using namespace masstransit_cpp;

	TEST_CASE("bus_config_tests", "[bus_config]")
	{
		SECTION( "ctor_make_context_info_from_json" ) 
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();
			uri uri("localhost", "guest", "guest");
			bus b;
			b
				.host(uri, [](send_endpoint& conf) {})
				.receive_endpoint(uri, "Test.AppName", [=](receive_endpoint & conf)
			{
				conf.consumer<message_mock>(consumer_mock);
				conf.poll_timeout(boost::posix_time::millisec(300));
			});
    	}

		SECTION("send_message_and_receive")
		{
			auto consumer_mock = std::make_shared<message_consumer_mock>();

			uri uri("localhost", "guest", "guest");
			bus b;
			b
				.host(uri, [](send_endpoint& conf) {})
				.auto_delete(true)
				.receive_endpoint(uri, "Test.AppName", [=](receive_endpoint & conf)
			{
				conf.consumer<message_mock>(consumer_mock);
				conf.auto_delete(true);
				conf.poll_timeout(boost::posix_time::seconds(6));
			});


			b.start();
			b.publish(message_mock(42));
			b.stop();

			REQUIRE(consumer_mock->saved_value.get_value_or(0) == 42);
		}
	}

}