#pragma once

/*
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MIT
 *
 * Copyright (c) 2010-2013 Alan Antonuk
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ***** END LICENSE BLOCK *****
 */

#include <map>
#include <string>
#include <vector>
#include <boost/variant.hpp>

#include <amqp.h>
#include <masstransit_cpp/global.hpp>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_table;
		struct MASSTRANSIT_CPP_API void_t {};

		inline bool MASSTRANSIT_CPP_API operator==(const void_t&, const void_t&) { return true; }

		using amqp_table_value = boost::variant<void_t, bool, int8_t, int16_t, int32_t, int64_t, float, double, std::string, amqp_table>;

		class MASSTRANSIT_CPP_API amqp_table : public std::map<std::string, amqp_table_value>
		{
		public:
			amqp_table();
			explicit amqp_table(amqp_table_t const& table);

			amqp_table_t to_amqp(std::shared_ptr<amqp_pool_t> & pool) const;
			amqp_table_t to_amqp(amqp_pool_t & pool) const;
			static amqp_table_t copy_amqp(amqp_table_t const& table, std::shared_ptr<amqp_pool_t> & pool);

		private:
			static amqp_table_t copy_amqp(amqp_table_t const& table, amqp_pool_t& pool);
			
			static amqp_table_value create_value(amqp_field_value_t const& entry);
			static amqp_field_value_t copy_value(amqp_field_value_t const& value, amqp_pool_t& pool);
		};
	}
}
