#include "message_consumer_depend.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>
#include <boost/di.hpp>

namespace mtc = masstransit_cpp;
namespace di = boost::di;

namespace boost {
	namespace di {
		template <>
		struct ctor_traits<masstransit_cpp_tests::bus_dependent_message_consumer> {
			BOOST_DI_INJECT_TRAITS(std::shared_ptr<mtc::i_publish_endpoint> const&);
		};
	}
}

namespace masstransit_cpp_tests
{
	template<class InjectorT>
	std::shared_ptr<mtc::bus> get_bus(InjectorT const& injector)
	{
		// singleton
		static auto bus = mtc::bus_factory::create_using_rabbit_mq([&injector](mtc::rabbit_mq_configurator & bus_configurator)
		{
			auto host = bus_configurator.host("localhost", [](mtc::amqp_host_configurator & host_configurator)
			{
				host_configurator.username("guest");
				host_configurator.password("guest");
			});

			bus_configurator.auto_delete(true);

			bus_configurator.receive_endpoint(host, "Test.AppName", [&injector](mtc::rabbit_mq::receive_endpoint_configurator & endpoint_configurator)
			{
				endpoint_configurator.load_from<message_mock>(injector);
				endpoint_configurator.poll_timeout(boost::posix_time::seconds(2));
				endpoint_configurator.auto_delete(true);
			});
		});

		return bus;
	}
	
	TEST_CASE("di_config_bus_then_send_2_message_using_deps_and_receive_tests", "[bus_config_2]")
	{
		auto container = di::make_injector(
			di::bind<mtc::i_publish_endpoint>().to([](auto const& injector) -> std::shared_ptr<mtc::i_publish_endpoint> { return get_bus(injector); }),
			di::bind<mtc::bus>().to([](auto const& injector) { return get_bus(injector); }),
			di::bind<mtc::message_consumer<message_mock>, bus_dependent_message_consumer>().to<bus_dependent_message_consumer>()
		);

		auto bus = container.create<std::shared_ptr<mtc::bus>>();
		auto consumer = container.create<std::shared_ptr<bus_dependent_message_consumer>>();

		bus->start();
		
		auto published = bus->publish(message_mock(bus_dependent_message_consumer::PUBLISH_WITH_BUS_DEPS)).get();
		consumer->wait();
		
		bus->stop();

		REQUIRE(published);
		REQUIRE(consumer->saved_value.get_value_or(0) == (bus_dependent_message_consumer::PUBLISH_WITH_BUS_DEPS + 1));
	}

	TEST_CASE("di_config_bus_then_send_2_message_using_context_and_receive_tests", "[bus_config_2]")
	{
		auto container = di::make_injector(
			di::bind<mtc::i_publish_endpoint>().to([](auto const& injector) -> std::shared_ptr<mtc::i_publish_endpoint> { return get_bus(injector); }),
			di::bind<mtc::bus>().to([](auto const& injector) { return get_bus(injector); }),
			di::bind<mtc::message_consumer<message_mock>, bus_dependent_message_consumer>().to<bus_dependent_message_consumer>()
		);

		auto bus = container.create<std::shared_ptr<mtc::bus>>();
		auto consumer = container.create<std::shared_ptr<bus_dependent_message_consumer>>();

		bus->start();
		
		auto published = bus->publish(message_mock(bus_dependent_message_consumer::PUBLISH_WITH_CONTEXT)).get();
		consumer->wait();
		
		bus->stop();

		REQUIRE(published);
		REQUIRE(consumer->saved_value.get_value_or(0) == (bus_dependent_message_consumer::PUBLISH_WITH_CONTEXT + 1));
	}
}
