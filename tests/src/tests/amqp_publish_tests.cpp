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

	TEST_CASE("amqp_publish")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("publish_success")
		{
			const auto message = std::make_shared<amqp_message>("message body");

			channel.publish("", "test_publish_rk", message);
		}

		SECTION("publish_large_message")
		{
			// Smallest frame size allowed by AMQP
			const auto small_channel = amqp_channel(host, 10, 4096);
			// Create a message with a body larger than a single frame
			const auto message = std::make_shared<amqp_message>(std::string(4099, 'a'));

			channel.publish("", "test_publish_rk", message);
		}

		SECTION("publish_badexchange")
		{
			const auto message = std::make_shared<amqp_message>("message body");

			REQUIRE_THROWS_AS(
				channel.publish("test_publish_notexist", "test_publish_rk", message),
				channel_exception);
		}

		SECTION("publish_recover_from_error")
		{
			const auto message = std::make_shared<amqp_message>("message body");

			REQUIRE_THROWS_AS(
				channel.publish("test_publish_notexist", "test_publish_rk", message),
				channel_exception);

			channel.publish("", "test_publish_rk", message);
		}

		SECTION("publish_mandatory_fail")
		{
			const auto message = std::make_shared<amqp_message>("message body");

			REQUIRE_THROWS_AS(
				channel.publish("", "test_publish_notexist", message, true),
				message_returned_exception);
		}

		SECTION("publish_mandatory_success")
		{
			const auto message = std::make_shared<amqp_message>("message body");
			const auto queue = channel.declare_queue("");

			channel.publish("", queue, message, true);
		}

		//SECTION("DISABLED_publish_immediate_fail1")
		//{
		//	const auto message = std::make_shared<amqp_message>("message body");

		//	// No queue connected
		//	REQUIRE_THROWS_AS(
		//		channel.publish("", "test_publish_notexist", message, false, true),
		//		message_returned_exception);
		//}

		//SECTION("DISABLED_publish_immediate_fail2")
		//{
		//	const auto message = std::make_shared<amqp_message>("message body");
		//	const auto queue = channel.declare_queue("");

		//	// No consumer connected
		//	REQUIRE_THROWS_AS(
		//		channel.publish("", queue, message, false, true),
		//		message_returned_exception);
		//}

		SECTION("publish_immediate_success")
		{
			const auto message = std::make_shared<amqp_message>("message body");
			const auto queue = channel.declare_queue("");
			auto consumer = channel.consume(queue, "");

			channel.publish("", queue, message, true);
		}
	}
}
