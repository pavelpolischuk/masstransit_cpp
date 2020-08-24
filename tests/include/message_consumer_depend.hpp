#pragma once
#include "message_mock.hpp"

#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>
#include <optional>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::message_consumer;
	using ::masstransit_cpp::consume_context;
	using ::masstransit_cpp::i_publish_endpoint;

	class message_consumer_depend : public message_consumer<message_mock>  // NOLINT(cppcoreguidelines-special-member-functions)
	{
	public:
		static const int PUBLISH_WITH_CONTEXT;

		message_consumer_depend() = default;
		~message_consumer_depend() override = default;

		std::optional<int> saved_value;

		void consume(consume_context<message_mock> const& context) override;
		void wait();

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
	};
}
