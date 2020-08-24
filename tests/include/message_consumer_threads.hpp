#pragma once
#include "message_mock.hpp"

#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>
#include <set>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::message_consumer;
	using ::masstransit_cpp::consume_context;
	using ::masstransit_cpp::i_publish_endpoint;

	class message_consumer_threads : public message_consumer<message_mock>
	{
	public:
		static const int NEED_THREAD_COUNT = 3;

		message_consumer_threads() = default;
		~message_consumer_threads() override = default;

		void consume(consume_context<message_mock> const& context) override;
		void wait();

		size_t involved_threads_count() const;

	private:
		mutable std::mutex mutex_;
		mutable std::condition_variable condition_;

		std::map<std::thread::id, int> values_by_threads_;
	};
}
