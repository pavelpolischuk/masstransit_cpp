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

	TEST_CASE("amqp_consume")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("basic_consume")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);
		}

		SECTION("basic_consume_bad_queue")
		{
			REQUIRE_THROWS_AS(channel.consume("test_consume_noexistqueue"), channel_exception);
		}

		SECTION("basic_consume_duplicate_tag")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);
			REQUIRE_THROWS_AS(channel.consume(queue, consumer), channel_exception);
		}

		SECTION("basic_cancel_consumer")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);
			channel.cancel(consumer);
		}

		SECTION("basic_cancel_bad_consumer")
		{
			REQUIRE_THROWS_AS(channel.cancel("test_consume_noexistconsumer"), consumer_tag_not_found_exception);
		}

		SECTION("basic_cancel_cancelled_consumer")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);
			channel.cancel(consumer);

			REQUIRE_THROWS_AS(channel.cancel(consumer), consumer_tag_not_found_exception);
		}

		SECTION("basic_consume_message")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue);
			channel.publish("", queue, message);

			std::shared_ptr<amqp_envelope> delivered;
			REQUIRE(channel.consume_message(consumer, delivered, std::chrono::microseconds::max()));
			REQUIRE(consumer == delivered->consumer_tag());
			REQUIRE("" == delivered->exchange());
			REQUIRE(queue == delivered->routing_key());
			REQUIRE(message->body() == delivered->message()->body());
		}

		SECTION("basic_consume_message_bad_consumer")
		{
			std::shared_ptr<amqp_envelope> envelope;
			REQUIRE_THROWS_AS(channel.consume_message("test_consume_noexistconsumer", envelope),
				consumer_tag_not_found_exception);
		}

		SECTION("basic_consume_inital_qos")
		{
			const auto message1 = std::make_shared<amqp_message>("Message1");
			const auto message2 = std::make_shared<amqp_message>("Message2");
			const auto message3 = std::make_shared<amqp_message>("Message3");

			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message1, true);
			channel.publish("", queue, message2, true);
			channel.publish("", queue, message3, true);

			const auto consumer = channel.consume(queue, "", true, false);
			std::shared_ptr<amqp_envelope> received1, received2;
			REQUIRE(channel.consume_message(consumer, received1, std::chrono::milliseconds(100)));

			REQUIRE_FALSE(channel.consume_message(consumer, received2, std::chrono::milliseconds(100)));
			channel.ack(received1->get_delivery_info());

			REQUIRE(channel.consume_message(consumer, received2, std::chrono::milliseconds(100)));
		}

		SECTION("basic_consume_2consumers")
		{
			const auto message1 = std::make_shared<amqp_message>("Message1");
			const auto message2 = std::make_shared<amqp_message>("Message2");
			const auto message3 = std::make_shared<amqp_message>("Message3");

			const auto queue1 = channel.declare_queue("");
			const auto queue2 = channel.declare_queue("");
			const auto queue3 = channel.declare_queue("");

			channel.publish("", queue1, message1);
			channel.publish("", queue2, message2);
			channel.publish("", queue3, message3);

			const auto consumer1 = channel.consume(queue1, "", true, false);
			const auto consumer2 = channel.consume(queue2, "", true, false);

			std::shared_ptr<amqp_envelope> envelope1;
			std::shared_ptr<amqp_envelope> envelope2;
			std::shared_ptr<amqp_envelope> envelope3;

			channel.consume_message(consumer1, envelope1);
			channel.ack(envelope1->get_delivery_info());
			channel.consume_message(consumer2, envelope2);
			channel.ack(envelope2->get_delivery_info());
			channel.get(envelope3, queue3);
			channel.ack(envelope3->get_delivery_info());
		}

		SECTION("basic_consume_1000messages")
		{
			const auto message1 = std::make_shared<amqp_message>("Message1");

			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue, "");

			std::shared_ptr<amqp_envelope> msg;
			for (int i = 0; i < 1000; ++i)
			{
				message1->set_timestamp(i);
				channel.publish("", queue, message1, true);
				channel.consume_message(consumer, msg);
			}
		}

		SECTION("basic_recover")
		{
			const auto message = std::make_shared<amqp_message>("Message1");

			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue, "", true, false);
			channel.publish("", queue, message);

			std::shared_ptr<amqp_envelope> message1;
			std::shared_ptr<amqp_envelope> message2;

			REQUIRE(channel.consume_message(consumer, message1));
			channel.recover(consumer);
			REQUIRE(channel.consume_message(consumer, message2));

			channel.delete_queue(queue);
		}

		SECTION("basic_recover_badconsumer")
		{
			REQUIRE_THROWS_AS(channel.recover("consumer_notexist"),
				consumer_tag_not_found_exception);
		}

		SECTION("basic_qos")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue, "", true, false);
			channel.publish("", queue, std::make_shared<amqp_message>("Message1"));
			channel.publish("", queue, std::make_shared<amqp_message>("Message2"));

			std::shared_ptr<amqp_envelope> incoming;
			REQUIRE(channel.consume_message(consumer, incoming, std::chrono::milliseconds(100)));
			REQUIRE_FALSE(channel.consume_message(consumer, incoming, std::chrono::milliseconds(100)));

			channel.qos(consumer, 2);
			REQUIRE(channel.consume_message(consumer, incoming, std::chrono::milliseconds(100)));

			channel.delete_queue(queue);
		}

		SECTION("basic_qos_badconsumer")
		{
			REQUIRE_THROWS_AS(channel.qos("consumer_notexist", 1),
				consumer_tag_not_found_exception);
		}

		SECTION("consumer_cancelled")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue, "", true, false);
			channel.delete_queue(queue);

			std::shared_ptr<amqp_envelope> env;
			REQUIRE_THROWS_AS(channel.consume_message(consumer, env),
				consumer_cancelled_exception);
		}

		SECTION("consumer_cancelled_one_message")
		{
			const auto queue = channel.declare_queue("");
			const auto consumer = channel.consume(queue, "", true, false);

			channel.publish("", queue, std::make_shared<amqp_message>("Message"));

			std::shared_ptr<amqp_envelope> env;
			channel.consume_message(consumer, env);

			channel.delete_queue(queue);

			REQUIRE_THROWS_AS(channel.consume_message(consumer, env),
				consumer_cancelled_exception);
		}

		SECTION("consume_multiple")
		{
			const auto queue1 = channel.declare_queue("");
			const auto queue2 = channel.declare_queue("");

			const auto body = "Message 1";
			channel.publish("", queue1, std::make_shared<amqp_message>(body));

			channel.consume(queue1);
			channel.consume(queue2);

			std::shared_ptr<amqp_envelope> env;
			channel.consume_message(env);

			REQUIRE(body == env->message()->body());
		}
	}
}
