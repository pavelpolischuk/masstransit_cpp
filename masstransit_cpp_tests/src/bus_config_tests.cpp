#include <message_consumer_mock.hpp>

#include <gtest/gtest.h>

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/uri.hpp>
#include <masstransit_cpp/send_endpoint.hpp>
#include <masstransit_cpp/receive_endpoint.hpp>

namespace masstransit_cpp_tests
{
	using namespace masstransit_cpp;

	TEST(bus_config_tests, ctor_make_context_info_from_json)
	{
		auto consumer_mock = std::make_shared<message_consumer_mock>();
		uri uri("localhost", "guest", "guest");
		bus b;
		b
			.host(uri, [](send_endpoint& conf) {})
			.receive_endpoint(uri, "Test.AppName", [=](receive_endpoint & conf)
		{
			conf.consumer(consumer_mock);
			conf.poll_timeout(boost::posix_time::millisec(300));
		});
	}

	void clean(uri const& uri, std::set<std::string> const& exchanges, std::set<std::string> const& queues)
	{
		auto channel = AmqpClient::Channel::CreateFromUri(uri.to_string());
		for(auto const& e : exchanges)
			channel->DeleteExchange(e);
		for(auto const& q : queues)
			channel->DeleteQueue(q);
	}

	TEST(bus_config_tests, send_message_and_receive)
	{
		auto consumer_mock = std::make_shared<message_consumer_mock>();

		uri uri("localhost", "guest", "guest");
		bus b;
		b
			.host(uri, [](send_endpoint& conf) {})
			.receive_endpoint(uri, "Test.AppName", [=](receive_endpoint & conf)
		{
			conf.consumer(consumer_mock);
			conf.poll_timeout(boost::posix_time::seconds(6));
		});


		b.start();
		b.publish(message_mock(42));
		b.stop();

		ASSERT_EQ(consumer_mock->saved_value.get_value_or(0), 42);
		clean(uri, b.exchanges(), {"Test.AppName"});
	}
}