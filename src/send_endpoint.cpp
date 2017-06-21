#include <masstransit_cpp/send_endpoint.hpp>
#include <masstransit_cpp/exchange_manager.hpp>
#include <masstransit_cpp/consume_context.hpp>

#include <boost/log/trivial.hpp>

namespace masstransit_cpp
{
	send_endpoint::send_endpoint(uri const& uri)
		: uri_(uri)
	{
	}

	send_endpoint& send_endpoint::host(host_info const& host)
	{
		host_ = host;
		return *this;
	}

	void send_endpoint::send(consume_context_info const& message, std::string const& type) const
	{
		auto for_send = message;
		for_send.send_host = host_;
		if (!exchange_manager_->has_exchange(type))
		{
			exchange_manager_->declare_message_type(type, queue_channel_);
		}

		auto body = for_send.to_json().dump(2);

		try
		{
			BOOST_LOG_TRIVIAL(debug) << "bus publish message:\n" << body;

			queue_channel_->BasicPublish(type, "", AmqpClient::BasicMessage::Create(body));

			BOOST_LOG_TRIVIAL(debug) << "[DONE]";
		}
		catch (std::exception & ex)
		{
			BOOST_LOG_TRIVIAL(error) << "send_endpoint::send\n\tException: " << ex.what();
		}
		catch (...)
		{
			BOOST_LOG_TRIVIAL(error) << "send_endpoint::send\n\tException: unknown";
		}
	}

	void send_endpoint::connect(std::shared_ptr<exchange_manager> const& exchange_manager)
	{
		queue_channel_ = AmqpClient::Channel::CreateFromUri(uri_.to_string());
		exchange_manager_ = exchange_manager;
	}
}
