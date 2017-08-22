#include "masstransit_cpp/receive_endpoint.hpp"
#include "masstransit_cpp/exchange_manager.hpp"

#include <boost/log/trivial.hpp>
#include <SimpleAmqpClient/SimpleAmqpClient.h>

namespace masstransit_cpp
{
	receive_endpoint::receive_endpoint(uri const& uri, std::string const& name)
		: queue_(name)
		, uri_(uri)
	{}

	receive_endpoint& receive_endpoint::poll_timeout(boost::posix_time::time_duration const& timeout)
	{
		auto ms = timeout.total_milliseconds();
		if (ms <= static_cast<int64_t>(std::numeric_limits<int>::max()))
			timeout_ = static_cast<int>(ms);
		else 
			BOOST_LOG_TRIVIAL(warning) << "receive_endpoint::poll_timeout: ms count is greater numeric_limits<int>::max\n";

		return *this;
	}

	receive_endpoint& receive_endpoint::auto_delete(bool is)
	{
		auto_delete_ = is;
		return *this;
	}

	receive_endpoint& receive_endpoint::prefetch_count(uint16_t count)
	{
		prefetch_count_ = count;
		return *this;
	}

	void receive_endpoint::connect(std::shared_ptr<exchange_manager> const& exchange_manager)
	{
		queue_channel_ = AmqpClient::Channel::CreateFromUri(uri_.to_string());
		queue_channel_->DeclareQueue(queue_, false, true, false, auto_delete_);
		
		for(auto const& c : consumers_by_type_)
		{
			auto type = c.second->message_type();
			exchange_manager->declare_message_type(type, queue_channel_);
			queue_channel_->BindQueue(queue_, type);
		}
		
		tag_ = queue_channel_->BasicConsume(queue_, "", true, false, true, prefetch_count_);
	}

	bool receive_endpoint::try_consume() const
	{
		AmqpClient::Envelope::ptr_t envelope;
		if (!queue_channel_->BasicConsumeMessage(tag_, envelope, timeout_))
			return false;

		if (envelope == nullptr)
			return false;

		auto message = envelope->Message();
		auto body = message->Body();
		auto body_j = nlohmann::json::parse(body.begin(), body.end());
		consume_context_info context(body_j);

		auto consumer = find_consumer(context.message_types);
		if (consumer == nullptr)
			return true;

		try
		{
			BOOST_LOG_TRIVIAL(debug) << "bus consumed message:\n" << body;

			consumer->consume(context);
			queue_channel_->BasicAck(envelope);

			BOOST_LOG_TRIVIAL(debug) << "[DONE]";
		}
		catch (std::exception & ex)
		{
			BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n" 
									 << body << "\n\tException: " << ex.what();
		}
		catch (...)
		{
			BOOST_LOG_TRIVIAL(error) << "when bus consumer[" << consumer->message_type() << "] try handle message:\n"
									 << body << "\n\tException: unknown";
		}

		return true;
	}

	std::shared_ptr<i_message_consumer> receive_endpoint::find_consumer(std::vector<std::string> const& message_types) const
	{
		for(auto const& message_type : message_types)
		{
			auto consumer = consumers_by_type_.find(message_type);
			if (consumer != consumers_by_type_.end())
				return consumer->second;
		}

		return nullptr;
	}
}
