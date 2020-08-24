#pragma once

#include <masstransit_cpp/global.hpp>

#include <set>
#include <memory>

namespace masstransit_cpp
{
	class i_error_handler;
	
	namespace rabbit_mq
	{
		class amqp_channel;

		class MASSTRANSIT_CPP_API exchange_manager
		{
		public:
			exchange_manager(bool auto_delete, std::shared_ptr<i_error_handler> error_handler);

			bool has_exchange(std::string const& name) const;

			void declare_exchange(std::string const& name, std::shared_ptr<amqp_channel> const& channel);

			std::set<std::string> const& all() const;

			static std::string get_name_by_message_type(std::string const& message_type);

		protected:
			const std::shared_ptr<i_error_handler> error_handler_;
			const bool auto_delete_{ false };
			
			std::set<std::string> exchanges_;
		};
	}
}
