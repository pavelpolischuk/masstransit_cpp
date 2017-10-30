#pragma once

#include <masstransit_cpp/global.hpp>

#include <set>
#include <boost/shared_ptr.hpp>

namespace masstransit_cpp
{
	namespace in_memory
	{
		class MASSTRANSIT_CPP_API exchange_manager
		{
		public:
			exchange_manager(bool auto_delete);

			bool has_exchange(std::string const& type) const;

			void declare_message_type(std::string const& type);

			std::set<std::string> const& all() const { return exchanges_; }

		protected:
			std::set<std::string> exchanges_;
			const bool auto_delete_{ false };
		};
	}
}