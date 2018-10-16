#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

namespace masstransit_cpp
{
	class datetime : public boost::posix_time::ptime
	{
	public:
		explicit datetime(boost::posix_time::ptime const& d)
			: boost::posix_time::ptime(d)
		{
		}

		static datetime now()
		{
			return datetime(boost::posix_time::microsec_clock::local_time());
		}

		template<class CharT>
		std::basic_string<CharT> to_string_type() const
		{
			if (time_of_day().is_special()) {
				return boost::gregorian::to_iso_extended_string_type<CharT>(date());
			}

			std::basic_ostringstream<CharT> ss;
			ss << boost::gregorian::to_iso_extended_string_type<CharT>(date());
			ss << 'T';
			ss << boost::posix_time::to_simple_string_type<CharT>(time_of_day());

			return ss.str();
		}
	};

	inline std::string to_string(datetime const& d)
	{
		return d.to_string_type<char>();
	}
}
