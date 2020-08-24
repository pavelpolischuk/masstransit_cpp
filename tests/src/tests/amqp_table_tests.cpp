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

#include "catch.hpp"

#include <masstransit_cpp/rabbit_mq/amqp_channel.hpp>
#include <masstransit_cpp/rabbit_mq/exceptions.hpp>
#include <boost/variant/get.hpp>
#include <algorithm>

namespace amqp_tests
{
	using namespace masstransit_cpp::rabbit_mq;

	TEST_CASE("amqp_table")
	{
		SECTION("convert_to_rabbitmq")
		{
			amqp_table table_in;
			table_in.emplace("void_key", void_t());
			table_in.emplace("bool_key", true);
			table_in.emplace("int8_key", int8_t(8));
			table_in.emplace("int16_key", int16_t(16));
			table_in.emplace("int32_key", int32_t(32));
			table_in.emplace("int64_key", int64_t(64));
			table_in.emplace("float_key", float(1.5));
			table_in.emplace("double_key", double(2.25));
			table_in.emplace("string_key", "A string!");

			amqp_table table_inner;
			table_inner.emplace("inner_string", "An inner table");

			table_in.emplace("table_key", table_inner);

			const auto message = std::make_shared<amqp_message>();
			message->set_header_table(table_in);

			REQUIRE(message->has_header_table());
			const auto table_out = message->header_table();
			REQUIRE(table_in.size() == table_out.size());
			REQUIRE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
		}

		SECTION("convert_to_rabbitmq_empty")
		{
			const amqp_table table_in;

			const auto message = std::make_shared<amqp_message>();
			message->set_header_table(table_in);

			const auto table_out = message->header_table();
			REQUIRE(0 == table_out.size());
		}

		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("basic_message_header_roundtrip")
		{
			amqp_table table_in;
			table_in.emplace("void_key", void_t());
			table_in.emplace("bool_key", true);
			table_in.emplace("int8_key", int8_t(8));
			table_in.emplace("int16_key", int16_t(16));
			table_in.emplace("int32_key", int32_t(32));
			table_in.emplace("int64_key", int64_t(64));
			table_in.emplace("float_key", float(1.5));
			table_in.emplace("double_key", double(2.25));
			table_in.emplace("string_key", "A string!");

			amqp_table table_inner;
			table_inner.emplace("inner_string", "An inner table");

			table_in.emplace("table_key", table_inner);

			const auto queue = channel.declare_queue("");
			const auto tag = channel.consume(queue, "");

			const auto message_in = std::make_shared<amqp_message>("Body");
			message_in->set_header_table(table_in);

			channel.publish("", queue, message_in);

			std::shared_ptr<amqp_envelope> envelope;
			channel.consume_message(tag, envelope);
			const auto message_out = envelope->message();
			const auto table_out = message_out->header_table();

			REQUIRE(table_in.size() == table_out.size());
			REQUIRE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
		}

		SECTION("basic_message_empty_table_roundtrip")
		{
			const auto queue = channel.declare_queue("");
			const auto tag = channel.consume(queue, "");

			amqp_table table_in;

			const auto message_in = std::make_shared<amqp_message>("Body");
			message_in->set_header_table(table_in);

			channel.publish("", queue, message_in);

			std::shared_ptr<amqp_envelope> envelope;
			channel.consume_message(tag, envelope);
			const auto message_out = envelope->message();
			const auto table_out = message_out->header_table();

			REQUIRE(table_in.size() == table_out.size());
			REQUIRE(std::equal(table_in.begin(), table_in.end(), table_out.begin()));
		}
	}
}
