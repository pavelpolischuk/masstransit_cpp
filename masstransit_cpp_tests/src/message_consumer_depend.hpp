#pragma once
#include "message_mock.hpp"

#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>
#include <boost/optional.hpp>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::message_consumer;
	using ::masstransit_cpp::consume_context;
	using ::masstransit_cpp::i_publish_endpoint;

	class bus_dependent_message_consumer : public message_consumer<message_mock>
	{
	public:
		static const int PUBLISH_WITH_BUS_DEPS = 142;
		static const int PUBLISH_WITH_CONTEXT = 242;

		boost::optional<int> saved_value;

		explicit bus_dependent_message_consumer(std::shared_ptr<i_publish_endpoint> const& bus);

		void consume(consume_context<message_mock> const& context) override;
		void wait();

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
		std::shared_ptr<i_publish_endpoint> bus_;
	};
}
