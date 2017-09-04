#include "message_mock.hpp"
#include "catch.hpp"

#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/bus_factory.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/rabbit_mq_configurator.hpp>
#include <masstransit_cpp/rabbit_mq/receive_endpoint_configurator.hpp>
#include <boost/optional/optional.hpp>
#include <boost/di.hpp>

namespace masstransit_cpp_tests
{
	namespace mtc = masstransit_cpp;
	namespace di = boost::di;

	template<class injector_t>
	std::shared_ptr<mtc::bus> get_bus(injector_t const& injector)
	{
		// singleton
		static auto bus = mtc::bus_factory::create_using_rabbit_mq([&injector](mtc::rabbit_mq_configurator & bus_configurator)
		{
			auto host = bus_configurator.host(mtc::amqp_uri("localhost"), [](mtc::amqp_host_configurator & host_configurator)
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

	class depend_message_consumer : public mtc::message_consumer<message_mock>
	{
	public:
		boost::optional<int> saved_value;

		BOOST_DI_INJECT(depend_message_consumer, std::shared_ptr<mtc::i_publish_endpoint> const& bus)
			: bus_(bus)
		{}
		
		~depend_message_consumer() override = default;

		void consume(mtc::consume_context<message_mock> const& context) override
		{
			if (context.message.id == 42)
			{
				bus_->publish(message_mock(43));
				return;
			}
			
			{
				std::unique_lock<std::mutex> lock(mutex_);
				saved_value = context.message.id; 
			}
				
			condition_.notify_all();
		}

		void wait()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			condition_.wait_for(lock, std::chrono::seconds(2), [this]{ return saved_value != boost::none; });
		}

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
		std::shared_ptr<mtc::i_publish_endpoint> bus_;
	};

	TEST_CASE("di_config_bus_then_send_message_and_receive_tests", "[bus_config_2]")
	{
		auto container = di::make_injector(
			di::bind<mtc::i_publish_endpoint>().to([](auto const& injector) -> std::shared_ptr<mtc::i_publish_endpoint> { return get_bus(injector); }),
			di::bind<mtc::bus>().to([](auto const& injector) { return get_bus(injector); }),
			di::bind<mtc::message_consumer<message_mock>, depend_message_consumer>().to<depend_message_consumer>()
		);

		auto bus = container.create<std::shared_ptr<mtc::bus>>();
		auto consumer = container.create<std::shared_ptr<depend_message_consumer>>();

		bus->start();
		
		auto published = bus->publish(message_mock(42)).get();
		consumer->wait();
		
		bus->stop();

		REQUIRE(published);
		REQUIRE(consumer->saved_value.get_value_or(0) == 43);
	}
}
