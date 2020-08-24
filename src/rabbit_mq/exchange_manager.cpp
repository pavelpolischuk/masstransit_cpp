#include "masstransit_cpp/rabbit_mq/exchange_manager.hpp"
#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"
#include "masstransit_cpp/utils/i_error_handler.hpp"

#include <regex>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		exchange_manager::exchange_manager(const bool auto_delete, std::shared_ptr<i_error_handler> error_handler)
			: error_handler_(std::move(error_handler))
			, auto_delete_(auto_delete)
		{}

		bool exchange_manager::has_exchange(std::string const& name) const
		{
			return exchanges_.find(name) != exchanges_.end();
		}

		void exchange_manager::declare_exchange(std::string const& name, std::shared_ptr<amqp_channel> const& channel)
		{
			if (has_exchange(name))
				return;
			
			try
			{
				channel->declare_exchange(name, amqp_channel::EXCHANGE_TYPE_FANOUT, false, true, auto_delete_);
				exchanges_.insert(name);
			}
			catch (std::exception const& ex)
			{
				error_handler_->on_error("exchange_manager::declare_exchange", ex.what());
			}
			catch (...)
			{
				error_handler_->on_error("exchange_manager::declare_exchange", "");
			}
		}

		std::set<std::string> const& exchange_manager::all() const
		{
			return exchanges_;
		}

		std::string exchange_manager::get_name_by_message_type(std::string const& message_type)
		{
			static const std::regex BRACES_REGEX("\\[|\\]");
			return std::regex_replace(message_type, BRACES_REGEX, "-");
		}
	}
}
