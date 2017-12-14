# Masstransit_cpp [![Build Status](https://travis-ci.org/pavelpolischuk/masstransit_cpp.svg?branch=master)](https://travis-ci.org/pavelpolischuk/masstransit_cpp)

AMQP message publishing and consuming library. Like and a little compatible with [MassTransit for .Net](https://github.com/MassTransit/MassTransit). It's using [SimpleAmqpClient](https://github.com/alanxz/SimpleAmqpClient).

### Quick start

#### Create message struct

It is necessary to implement default `ctor`, static method `message_type()` and serialize/deserialize methods `to_json`/`from_json`.

```cpp
#include <masstransit_cpp/json.hpp>

struct my_message {
    int data_1{ -1 };
    std::string data_2;

    my_message(){}

    static std::string message_type() { return "MyMessage"; }
};

void to_json(nlohmann::json & j, my_message const& p) {
    j = nlohmann::json{ {"data_1", p.data_1}, {"data_2", p.data_2} };
}

void from_json(nlohmann::json const& j, person & p) {
    p.data_1 = j.at("data_1").get<int>();
    p.data_2 = j.at("data_2").get<std::string>();
}
```

#### Create message consumer

Consumer have to extend `masstransit_cpp::message_consumer<message_t>` and override `void consume(masstransit_cpp::consume_context<message_t> const&)`.

```cpp
#include <my_message.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <iostream>

class my_message_consumer : public masstransit_cpp::message_consumer<my_message> {
public:	
    void consume(masstransit_cpp::consume_context<my_message> const& context) override {
        std::cout << "Consume " << context.message.data_1 << ":" << context.message.data_2 << std::endl;
    }
};
```

#### Configure and start bus

```cpp
#include <my_consumer.hpp>
#include <masstransit_cpp/bus.hpp>

using namespace masstransit_cpp;

void main() {

auto one_consumer = std::make_shared<my_consumer>();
auto bus = masstransit_cpp::bus_factory::create_using_rabbit_mq([=](masstransit_cpp::rabbit_mq_configurator & bus_configurator) {
        auto host = bus_configurator.host("127.0.0.1", [](auto & host_configurator) {
            host_configurator.username("user");
            host_configurator.password("password");
        });

        bus_configurator.receive_endpoint(host, "QueueName", [=](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator) {
            endpoint_configurator.consumer<my_message>(one_consumer);   // set consumer for message

            endpoint_configurator.handler<my_message>(                  // OR set handler for message
                        [=](masstransit_cpp::consume_context<my_message> const& context) {
                            std::cout << "Handle " << context.message.data_1 << ":" << context.message.data_2 << std::endl;
                        });
        });
    });

    b->start();
//  ... wait ...
    b->stop();
}
```

#### Publish message

Bus implement interface `masstransit_cpp::i_bus` and has template method `void publish<message_t>(message_t const&)`.
You need pass your configured `bus` as dependence where you want to publish message.

```cpp
void publish_foo(std::shared_ptr<i_publish_endpoint> const& bus) {
    my_message m;
    
    m.data_1 = 42;
    m.data_2 = "qwerty";
    
    std::future<bool> task = bus->publish(m);
}
```

#### Configure consumers using Boost.DI

If you use dependency injection, your consumers may depends on bus (handle some message and publish new one). You can configure bus use Boost.DI.

```cpp
#include <my_message.hpp>
#include <masstransit_cpp/bus.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <boost/di.hpp>

struct next_message {
    int data{ -1 };

    next_message() {}
    explicit next_message(int data) : data(data) {}
	
    static std::string message_type() { return "NextMessage"; }
};

void to_json(nlohmann::json & j, my_message const& p) {
    j = nlohmann::json{ {"data", p.data} };
}

void from_json(nlohmann::json const& j, person & p) {
    p.data = j.at("data").get<int>();
}

class dependent_consumer : public masstransit_cpp::message_consumer<my_message> {
    std::shared_ptr<masstransit_cpp::i_publish_endpoint> bus_;

public:
    BOOST_DI_INJECT(dependent_consumer, std::shared_ptr<masstransit_cpp::i_publish_endpoint> const& bus)
        : bus_(bus) {}
	
    void consume(masstransit_cpp::consume_context<my_message> const& context) override {
        bus_->publish(next_message(43));
    }
};


template<class injector_t>
std::shared_ptr<masstransit_cpp::bus> create_bus(injector_t const& injector) {   // method to create singleton bus using injector
    static auto bus =  masstransit_cpp::bus_factory::create_using_rabbit_mq([&injector](masstransit_cpp::rabbit_mq_configurator & bus_configurator) {
        auto host = bus_configurator.host("localhost", [](masstransit_cpp::amqp_host_configurator & host_configurator) {
            host_configurator.username("user");
            host_configurator.password("password");
        });

        bus_configurator.receive_endpoint(host, "QueueName", 
                [&injector](masstransit_cpp::rabbit_mq::receive_endpoint_configurator & endpoint_configurator) {
                                                                                // load consumer for my_message from injector,
                    endpoint_configurator.load_from<my_message>(injector);      // i.e. impl of masstransit_cpp::message_consumer<my_message>
                });                                                             
    });
	
    return bus;
}

void main() {
    auto container = boost::di::make_injector(
            boost::di::bind<masstransit_cpp::message_consumer<my_message>, dependent_consumer>().to<dependent_consumer>(),
            boost::di::bind<masstransit_cpp::i_publish_endpoint>().to([](auto const& injector) -> std::shared_ptr<masstransit_cpp::i_publish_endpoint> { return get_bus(injector); }),
            boost::di::bind<masstransit_cpp::bus>().to([](auto const& injector) { return get_bus(injector); })
        );

    auto bus = container.create<std::shared_ptr<masstransit_cpp::bus>>();
		
    bus->start();
    bus->publish(my_message()).get();
    bus->stop();
}
```

