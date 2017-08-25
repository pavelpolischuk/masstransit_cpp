#pragma once

#include <masstransit_cpp/global.hpp>

#include <set>
#include <boost/shared_ptr.hpp>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_EXPORT exchange_manager
	{
	public:
		inline bool has_exchange(std::string const& type) const { return exchanges_.count(type) > 0; }

		void declare_message_type(std::string const& type, boost::shared_ptr<AmqpClient::Channel> const& channel);
		void auto_delete(bool is);

		std::set<std::string> const& all() const { return exchanges_; }

	protected:
		std::set<std::string> exchanges_;
		bool auto_delete_{ false };
	};
}