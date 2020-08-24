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

	TEST_CASE("amqp_queue")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("queue_declare")
		{
			const auto queue = channel.declare_queue("");
			channel.delete_queue(queue);
		}

		SECTION("queue_declare_named")
		{
			const auto queue = channel.declare_queue("declare_queue_test");
			REQUIRE("declare_queue_test" == queue);

			channel.delete_queue(queue);
		}

		SECTION("queue_declare_passive")
		{
			const auto queue = channel.declare_queue("declare_queue_passive");
			channel.declare_queue("declare_queue_passive", true);
			channel.delete_queue(queue);
		}

		SECTION("queue_declare_passive_fail")
		{
			REQUIRE_THROWS_AS(
				channel.declare_queue("declare_queue_notexist", true),
				channel_exception);
		}

		SECTION("queue_declare_durable")
		{
			const auto queue = channel.declare_queue("declare_queue_durable", false, true, false);

			channel.delete_queue(queue);
		}

		SECTION("queue_declare_notexclusive")
		{
			const auto queue = channel.declare_queue("declare_queue_not_exclusive", false, false, false);

			channel.delete_queue(queue);
		}

		SECTION("queue_declare_notautodelete")
		{
			const auto queue = channel.declare_queue("declare_queue_not_auto_delete", false, false, false, false);

			channel.delete_queue(queue);
		}

		SECTION("queue_declare_counts")
		{
			uint32_t message_count = 123;
			uint32_t consumer_count = 123;

			const auto queue = channel.declare_queue_with_counts(
				"queue_declare_counts", message_count, consumer_count);

			REQUIRE("" != queue);
			REQUIRE(0 == message_count);
			REQUIRE(0 == consumer_count);

			const std::string body("Test Message");
			const auto out_message = std::make_shared<amqp_message>(body);
			channel.publish("", queue, out_message);
			channel.publish("", queue, out_message);
			channel.publish("", queue, out_message);

			const auto queue2 = channel.declare_queue_with_counts(
				"queue_declare_counts", message_count, consumer_count);

			REQUIRE("" != queue2);
			REQUIRE(3 == message_count);
			REQUIRE(0 == consumer_count);

			channel.delete_queue(queue);
		}

		SECTION("queue_declare_counts_table")
		{
			uint32_t message_count = 123;
			uint32_t consumer_count = 123;

			amqp_table args;
			args.emplace("IsATest", true);

			const auto queue = channel.declare_queue_with_counts(
				"queue_declare_counts_table", message_count, consumer_count, false, false,
				true, true, args);

			REQUIRE("" != queue);
			REQUIRE(0 == message_count);
			REQUIRE(0 == consumer_count);

			const std::string body("Test Message");
			const auto out_message = std::make_shared<amqp_message>(body);
			channel.publish("", queue, out_message);
			channel.publish("", queue, out_message);
			channel.publish("", queue, out_message);

			const auto queue2 = channel.declare_queue_with_counts(
				"queue_declare_counts_table", message_count, consumer_count, false, false,
				true, true, args);

			REQUIRE("" != queue2);
			REQUIRE(3 == message_count);
			REQUIRE(0 == consumer_count);

			channel.delete_queue(queue);
		}

		SECTION("queue_delete")
		{
			const auto queue = channel.declare_queue("delete_queue");
			channel.delete_queue(queue);
			REQUIRE_THROWS_AS(channel.declare_queue(queue, true), channel_exception);
		}

		/*SECTION("queue_delete_badqueue")
		{
			REQUIRE_THROWS_AS(channel.delete_queue("delete_queue_not_exist"), channel_exception);
		}*/

		SECTION("queue_delete_ifunused")
		{
			const auto queue = channel.declare_queue("delete_queue_if_unused");
			channel.delete_queue(queue, true);
			REQUIRE_THROWS_AS(channel.declare_queue(queue, true), channel_exception);
		}

		SECTION("queue_delete_ifused")
		{
			const auto queue = channel.declare_queue("delete_queue_if_used");
			channel.consume(queue);
			REQUIRE_THROWS_AS(channel.delete_queue(queue, true), channel_exception);

			channel.delete_queue(queue);
		}

		SECTION("queue_delete_ifempty")
		{
			const auto queue = channel.declare_queue("delete_queue_if_empty");
			channel.delete_queue(queue, false, true);

			REQUIRE_THROWS_AS(channel.declare_queue(queue, true), channel_exception);
		}

		SECTION("queue_delete_ifnotempty")
		{
			const auto queue = channel.declare_queue("delete_queue_if_not_empty");
			const auto message = std::make_shared<amqp_message>("Message body");
			channel.publish("", queue, message, true);

			REQUIRE_THROWS_AS(channel.delete_queue(queue, false, true), channel_exception);

			channel.delete_queue(queue);
		}

		SECTION("queue_bind")
		{
			channel.declare_exchange("queue_bind_exchange");
			const auto queue = channel.declare_queue("queue_bind_queue");

			channel.bind_queue(queue, "queue_bind_exchange", "rk");

			channel.delete_exchange("queue_bind_exchange");
			channel.delete_queue(queue);
		}

		SECTION("queue_bind_badexchange")
		{
			const auto queue = channel.declare_queue("queue_bind_bad_exchange");

			REQUIRE_THROWS_AS(
				channel.bind_queue(queue, "queue_bind_exchangenotexist", "rk"),
				channel_exception);

			channel.delete_queue(queue);
		}

		SECTION("queue_bind_badqueue")
		{
			channel.declare_exchange("queue_bind_bad_queue");

			REQUIRE_THROWS_AS(
				channel.bind_queue("queue_bind_queuenotexist", "queue_bind_badqueue", "rk"),
				channel_exception);

			channel.delete_exchange("queue_bind_bad_queue");
		}

		SECTION("queue_bind_nokey")
		{
			channel.declare_exchange("queue_bind_exchange");
			const auto queue = channel.declare_queue("queue_bind_queue");

			channel.bind_queue(queue, "queue_bind_exchange");

			channel.delete_exchange("queue_bind_exchange");
			channel.delete_queue(queue);
		}

		SECTION("queue_unbind")
		{
			channel.declare_exchange("queue_unbind_exchange");
			const auto queue = channel.declare_queue("queue_unbind_queue");
			channel.bind_queue(queue, "queue_unbind_exchange", "rk");

			channel.unbind_queue(queue, "queue_unbind_exchange", "rk");

			channel.delete_exchange("queue_unbind_exchange");
			channel.delete_queue(queue);
		}

		/*SECTION("queue_unbind_badbinding")
		{
			REQUIRE_THROWS_AS(
				channel.unbind_queue("queue_unbind_queuenotexist", "queue_unbind_exchangenotexist", "rk"),
				channel_exception);
		}*/

		SECTION("queue_purge")
		{
			auto queue = channel.declare_queue("queue_purge");
			const auto message = std::make_shared<amqp_message>("Message Body");
			channel.publish("", queue, message, true);

			channel.purge_queue(queue);
			std::shared_ptr<amqp_envelope> envelope;
			REQUIRE_FALSE(channel.get(envelope, queue));

			channel.delete_queue(queue);
		}

		SECTION("queue_purge_badqueue")
		{
			REQUIRE_THROWS_AS(
				channel.purge_queue("purge_queue_queuenotexist"),
				channel_exception);
		}
	}
}
