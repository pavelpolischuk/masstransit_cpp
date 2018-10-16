#pragma once

#include <masstransit_cpp/global.hpp>

#include <set>
#include <boost/shared_ptr.hpp>

namespace AmqpClient { class Channel; }

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class MASSTRANSIT_CPP_API exchange_manager
		{
		public:
			exchange_manager(bool auto_delete);

			bool has_exchange(std::string const& name) const;

			void declare_exchange(std::string const& name, boost::shared_ptr<AmqpClient::Channel> const& channel);
			
			std::set<std::string> const& all() const { return exchanges_; }

		protected:
			std::set<std::string> exchanges_;
			const bool auto_delete_{ false };
		};
	}
}
