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

#include <array>
#include <algorithm>
#include <iostream>

namespace amqp_tests
{
	using namespace masstransit_cpp::rabbit_mq;

	TEST_CASE("amqp_message")
	{
		SECTION("empty_message")
		{
			const auto empty_message = std::make_shared<amqp_message>();

			REQUIRE(std::string() == empty_message->body());

			// Allow the message to destruct
		}

		SECTION("empty_message_add_body")
		{
			const auto empty_message = std::make_shared<amqp_message>();

			REQUIRE(std::string() == empty_message->body());

			const std::string body("Message Body");
			empty_message->set_body(body);

			REQUIRE(body == empty_message->body());

			// Allow the message to destruct
		}

		SECTION("empty_message_add_body2")
		{
			const auto empty_message = std::make_shared<amqp_message>();

			REQUIRE(std::string() == empty_message->body());

			const std::string body("Message Body");
			empty_message->set_body(body);

			REQUIRE(body == empty_message->body());

			const std::string body2("Second body");
			empty_message->set_body(body2);
			REQUIRE(body2 == empty_message->body());

			// Allow the message to destruct
		}

		SECTION("initial_message_replace")
		{
			const std::string first_body("First message Body");
			const auto message = std::make_shared<amqp_message>(first_body);

			REQUIRE(first_body == message->body());

			const std::string second_body("Second message Body");
			message->set_body(second_body);

			REQUIRE(second_body == message->body());

			// Allow the message to destruct
		}

		SECTION("initial_message_replace2")
		{
			const std::string first_body("First message body");
			const auto message = std::make_shared<amqp_message>(first_body);
			REQUIRE(first_body == message->body());

			const std::string second_body("second message body");
			message->set_body(second_body);
			REQUIRE(second_body == message->body());

			const std::string third_body("3rd Body");
			message->set_body(third_body);
			REQUIRE(third_body == message->body());
		}

		SECTION("embedded_nulls")
		{
			const std::array<char, 7> message_data = {{'a', 'b', 'c', 0, '1', '2', '3'}};
			const std::string body(message_data.data(), message_data.size());
			const auto message = std::make_shared<amqp_message>(body);
			REQUIRE(body == message->body());

			auto amqp_body = message->get_amqp_body();
			REQUIRE(body.length() == amqp_body.len);
			REQUIRE(std::equal(message_data.begin(), message_data.end(), reinterpret_cast<char *>(amqp_body.bytes)));

			const std::array<char, 7> message_data2 = {{'1', '2', '3', 0, 'a', 'b', 'c'}};
			const std::string body2(message_data2.data(), message_data2.size());
			message->set_body(body2);
			REQUIRE(body2 == message->body());

			auto amqp_body2 = message->get_amqp_body();
			REQUIRE(body2.length() == amqp_body2.len);
			REQUIRE(std::equal(message_data2.begin(), message_data2.end(), reinterpret_cast<char *>(amqp_body2.bytes)));
		}

		SECTION("replaced_received_body")
		{
			const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
			const auto channel = amqp_channel(host);

			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);

			const std::string body("First Message Body");
			const auto out_message = std::make_shared<amqp_message>(body);
			channel.publish("", queue, out_message);

			std::shared_ptr<amqp_envelope> envelope;
			channel.consume_message(consumer, envelope);
			const auto in_message = envelope->message();
			REQUIRE(out_message->body() == in_message->body());

			const std::string body2("Second message body");
			in_message->set_body(body2);
			REQUIRE(body2 == in_message->body());
		}
	}
}
