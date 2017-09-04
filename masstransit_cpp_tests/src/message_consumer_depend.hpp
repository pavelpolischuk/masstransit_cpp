#include "message_mock.hpp"

#include <masstransit_cpp/message_consumer.hpp>
#include <masstransit_cpp/i_publish_endpoint.hpp>
#include <boost/optional.hpp>

namespace masstransit_cpp_tests
{
	using ::masstransit_cpp::message_consumer;
	using ::masstransit_cpp::consume_context;
	using ::masstransit_cpp::i_publish_endpoint;

	class message_consumer_depend : public message_consumer<message_mock>
	{
	public:
		boost::optional<int> saved_value;

		message_consumer_depend(std::shared_ptr<i_publish_endpoint> const& bus);

		void consume(consume_context<message_mock> const& context) override;
		void wait();

	private:
		std::mutex mutex_;
		std::condition_variable condition_;
		std::shared_ptr<i_publish_endpoint> bus_;
	};
}
