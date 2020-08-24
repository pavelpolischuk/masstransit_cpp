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

	TEST_CASE("amqp_channels")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		auto channel = amqp_channel(host);

		SECTION("first_channel")
		{
			channel.declare_exchange("test_channel_exchange", amqp_channel::EXCHANGE_TYPE_FANOUT, false, false, true);
			channel.delete_exchange("test_channel_exchange");
		}

		// Check to see that channels are reused properly
		SECTION("channel_reuse")
		{
			channel.declare_exchange("test_channel_exchange1", amqp_channel::EXCHANGE_TYPE_FANOUT, false, false, true);
			channel.declare_exchange("test_channel_exchange2", amqp_channel::EXCHANGE_TYPE_FANOUT, false, false, true);
			channel.delete_exchange("test_channel_exchange1");
			channel.delete_exchange("test_channel_exchange2");
		}

		// Check to see that a new channel is created when a channel is put in an exception state
		SECTION("channel_recover_from_error")
		{
			REQUIRE_THROWS_AS(
				channel.declare_exchange("test_channel_exchangedoesnotexist", amqp_channel::EXCHANGE_TYPE_FANOUT, true, false, true),
				channel_exception);

			channel.declare_exchange("test_channel_exchange", amqp_channel::EXCHANGE_TYPE_FANOUT, false, false, true);
			channel.delete_exchange("test_channel_exchange");
		}

		SECTION("channel_publish_success1")
		{
			const auto message = std::make_shared<amqp_message>("Test message");

			channel.publish("", "test_channel_routingkey", message, false, false);
		}

		SECTION("channel_publish_success2")
		{
			const auto message = std::make_shared<amqp_message>("Test message");

			channel.publish("", "test_channel_routingkey", message, false, false);
			channel.publish("", "test_channel_routingkey", message, false, false);
		}

		SECTION("channel_publish_returned_mandatory")
		{
			const auto message = std::make_shared<amqp_message>("Test message");

			REQUIRE_THROWS_AS(
				channel.publish("", "test_channel_noqueue", message, true, false),
				message_returned_exception);
		}

		/*SECTION("DISABLED_channel_publish_returned_immediate")
		{
			const auto message = std::make_shared<amqp_message>("Test message");
			const auto queue_name = channel.declare_queue("");

			REQUIRE_THROWS_AS(
				channel.publish("", queue_name, message, false, true),
				message_returned_exception);
		}*/

		SECTION("channel_publish_bad_exchange")
		{
			const auto message = std::make_shared<amqp_message>("Test message");

			REQUIRE_THROWS_AS(
				channel.publish("test_channel_badexchange", "test_channel_rk", message, false, false),
				channel_exception);
		}

		SECTION("channel_publish_bad_exchange_recover")
		{
			const auto message = std::make_shared<amqp_message>("Test message");

			REQUIRE_THROWS_AS(
				channel.publish("test_channel_badexchange", "test_channel_rk", message, false, false),
				channel_exception);

			channel.publish("", "test_channel_rk", message, false, false);
		}

		SECTION("channel_consume_success")
		{
			const auto message = std::make_shared<amqp_message>("Test message");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message);

			auto consumer = channel.consume(queue);

			std::shared_ptr<amqp_envelope> consumed_envelope;
			REQUIRE(channel.consume_message(consumer, consumed_envelope));
		}

		SECTION("channel_consume_success_timeout")
		{
			const auto message = std::make_shared<amqp_message>("Test message");
			const auto queue = channel.declare_queue("");

			const auto consumer = channel.consume(queue, "", true, false);
			channel.publish("", queue, message);

			std::shared_ptr<amqp_envelope> consumed_envelope;
			REQUIRE(channel.consume_message(consumer, consumed_envelope, std::chrono::milliseconds(5000)));
		}

		SECTION("big_message")
		{
			const auto message = std::make_shared<amqp_message>(std::string(4099, 'a'));
			const auto queue = channel.declare_queue("");

			const auto consumer = channel.consume(queue);
			channel.publish("", queue, message);

			std::shared_ptr<amqp_envelope> consumed_envelope;
			REQUIRE(channel.consume_message(consumer, consumed_envelope));
		}
	}
}
