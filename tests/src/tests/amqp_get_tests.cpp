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

namespace amqp_tests
{
	using namespace masstransit_cpp::rabbit_mq;

	TEST_CASE("amqp_get")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("get_ok")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message, true);

			std::shared_ptr<amqp_envelope> new_message;
			REQUIRE(channel.get(new_message, queue));
			REQUIRE(message->body() == new_message->message()->body());
		}

		SECTION("get_empty")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");

			std::shared_ptr<amqp_envelope> new_message;
			REQUIRE_FALSE(channel.get(new_message, queue));
		}

		SECTION("get_big")
		{
			// Smallest frame size allowed by AMQP
			const auto small_channel = amqp_channel(host, 10, 4096);

			// Create a message with a body larger than a single frame
			const auto message = std::make_shared<amqp_message>(std::string(4099, 'a'));
			const auto queue = small_channel.declare_queue("");

			small_channel.publish("", queue, message);
			std::shared_ptr<amqp_envelope> new_message;
			REQUIRE(small_channel.get(new_message, queue));
			REQUIRE(message->body() == new_message->message()->body());
		}

		SECTION("bad_queue")
		{
			std::shared_ptr<amqp_envelope> new_message;
			REQUIRE_THROWS_AS(
				channel.get(new_message, "test_get_nonexistantqueue"),
				channel_exception);
		}

		SECTION("ack_message")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message, true);

			std::shared_ptr<amqp_envelope> new_message;
			REQUIRE(channel.get(new_message, queue, false));
			channel.ack(new_message->get_delivery_info());
			REQUIRE_FALSE(channel.get(new_message, queue, false));
		}
	}
}
