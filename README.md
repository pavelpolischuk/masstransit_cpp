
# Masstransit_cpp

AMQP message publishing and consuming library. Like and a little compatible with Masstransit for .Net.

### Quick start

##### Create message struct

It is necessary to implement `ctor` from `nlohmann::json`, static method `message_type()->std::string` and serialize method `to_json()->nlohmann::json`.

```cpp
#include <masstransit_cpp/json.hpp>

struct my_message
{
	int data_1;
	std::string data_2;

	my_message(nlohmann::json const& obj);
	
	static std::string message_type() { return "MyMessage"; }

	nlohmann::json to_json() { return { {"data_1", data_1}, {"data_2", data_2} };
};
```

##### Create message consumer

Consumer have to extend `masstrasit_cpp::message_consumer<message_t>` and override `void consume(masstrasit_cpp::consume_context<message_t> const&)`.

```cpp
#include <my_message.hpp>
#include <masstransit_cpp/message_consumer.hpp>
#include <iostream>

class my_message_consumer : public masstrasit_cpp::message_consumer<my_message>
{
public:
	~my_message_consumer() override {};
	
	void consume(masstrasit_cpp::consume_context<my_message> const& context) override
	{
	    std::cout << "Consume " << context.message.data_1 << ":" << context.message.data_2 << std::endl;
	}
};
```

##### Configure and start bus

```cpp
#include <my_consumer.hpp>
#include <masstransit_cpp/bus.hpp>

auto one_consumer = std::make_shared<my_consumer>();

uri uri("127.0.0.1", "user", "password");

auto bus = std::make_shared<masstransit_cpp::bus>();
bus->host(uri, [](send_endpoint& conf) {})
	.receive_endpoint(uri, "QueueName", [=](receive_endpoint & conf)
{
	conf.consumer(one_consumer);
	conf.poll_timeout(boost::posix_time::seconds(1));
});

b->start();
```

##### Publish message

Bus implement interface `i_bus` and has template method `void publish<message_t>(message_t const&)`.
You need pass your configured `bus` as dependence where you want to publish message. 

```cpp

void publish_foo(std::shared_ptr<masstransit_cpp::i_bus> const& bus)
{
    my_message m;
    
    m.data_1 = 42;
    m.data_2 = "qwerty";
    
    bus->publish(m);
};
```

##### And stop bus

```cpp
b->stop();
```


