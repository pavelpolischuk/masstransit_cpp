#pragma once

#include <masstransit_cpp/global.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace masstransit_cpp
{
	class MASSTRANSIT_CPP_API datetime : public std::chrono::system_clock::time_point
	{
	public:
		explicit datetime(std::chrono::system_clock::time_point const& d);

		static datetime now();

		template<class CharT = char>
		std::basic_string<CharT> to_string() const
		{
			std::basic_ostringstream<CharT> ss;
			ss << *this;
			return ss.str();
		}

		template<class CharT>
		friend std::basic_ostream<CharT>& operator << (std::basic_ostream<CharT> & stream, datetime const& dt)
		{
			const auto tt = std::chrono::system_clock::to_time_t(dt);
			const auto gmt = *std::gmtime(&tt);
			return stream << std::put_time(&gmt, "%FT%TZ");
		}
	};
}
