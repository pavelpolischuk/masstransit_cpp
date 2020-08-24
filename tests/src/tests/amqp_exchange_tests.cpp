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

	TEST_CASE("amqp_exchange")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("declare_exchange_defaults")
		{
			channel.declare_exchange("declare_defaults");
			channel.delete_exchange("declare_defaults");
		}

		SECTION("declare_exchange_direct")
		{
			channel.declare_exchange("declare_direct", amqp_channel::EXCHANGE_TYPE_DIRECT);
			channel.delete_exchange("declare_direct");
		}

		SECTION("declare_exchange_fanout")
		{
			channel.declare_exchange("declare_fanout", amqp_channel::EXCHANGE_TYPE_FANOUT);
			channel.delete_exchange("declare_fanout");
		}

		SECTION("declare_exchange_topic")
		{
			channel.declare_exchange("declare_topic", amqp_channel::EXCHANGE_TYPE_TOPIC);
			channel.delete_exchange("declare_topic");
		}

		SECTION("declare_exchange_passive_good")
		{
			channel.declare_exchange("declare_passive", amqp_channel::EXCHANGE_TYPE_DIRECT);
			channel.declare_exchange("declare_passive", amqp_channel::EXCHANGE_TYPE_DIRECT, true);

			channel.delete_exchange("declare_passive");
		}

		SECTION("declare_exchange_passive_notexist")
		{
			REQUIRE_THROWS_AS(
				channel.declare_exchange("declare_passive_notexist", amqp_channel::EXCHANGE_TYPE_DIRECT, true),
				channel_exception);
		}

		SECTION("declare_exchange_typemismatch")
		{
			channel.declare_exchange("declare_typemismatch", amqp_channel::EXCHANGE_TYPE_DIRECT);
			REQUIRE_THROWS_AS(
				channel.declare_exchange("declare_typemismatch", amqp_channel::EXCHANGE_TYPE_FANOUT),
				channel_exception);

			channel.delete_exchange("declare_typemismatch");
		}

		SECTION("declare_exchange_typemismatch2")
		{
			channel.declare_exchange("declare_typemismatch", amqp_channel::EXCHANGE_TYPE_DIRECT);
			REQUIRE_THROWS_AS(
				channel.declare_exchange("declare_typemismatch", amqp_channel::EXCHANGE_TYPE_DIRECT, false, true),
				channel_exception);

			channel.delete_exchange("declare_typemismatch");
		}

		SECTION("declare_exchange_durable")
		{
			channel.declare_exchange("declare_durable", amqp_channel::EXCHANGE_TYPE_DIRECT, false, true);
			channel.delete_exchange("declare_durable");
		}

		SECTION("declare_exchange_autodelete")
		{
			channel.declare_exchange("declare_autodelete", amqp_channel::EXCHANGE_TYPE_DIRECT, false, false, true);
			channel.delete_exchange("declare_autodelete");
		}

		SECTION("delete_exchange")
		{
			channel.declare_exchange("delete_exchange");
			channel.delete_exchange("delete_exchange");
		}

		/*SECTION("delete_exchange_not_exist")
		{
			REQUIRE_THROWS_AS(channel.delete_exchange("exchange_not_exist"), channel_exception);
		}*/

		SECTION("delete_exhange_ifunused")
		{
			channel.declare_exchange("exchange_used", amqp_channel::EXCHANGE_TYPE_DIRECT);
			channel.delete_exchange("exchange_used", true);
		}

		SECTION("delete_exhange_ifused")
		{
			channel.declare_exchange("exchange_used", amqp_channel::EXCHANGE_TYPE_DIRECT);
			const auto queue = channel.declare_queue("");
			channel.bind_queue(queue, "exchange_used", "whatever");

			REQUIRE_THROWS_AS(channel.delete_exchange("exchange_used", true), channel_exception);

			channel.delete_queue(queue);
			channel.delete_exchange("exchange_used");
		}

		SECTION("bind_exchange")
		{
			channel.declare_exchange("exchange_bind_dest");
			channel.declare_exchange("exchange_bind_src");

			channel.bind_exchange("exchange_bind_dest", "exchange_bind_src", "rk");

			channel.delete_exchange("exchange_bind_dest");
			channel.delete_exchange("exchange_bind_src");
		}

		SECTION("bind_exchange_badexchange")
		{
			channel.declare_exchange("exchange_bind_dest");

			REQUIRE_THROWS_AS(
				channel.bind_exchange("exchange_bind_dest", "exchange_bind_notexist", "rk"),
				channel_exception);

			channel.delete_exchange("exchange_bind_dest");
		}

		SECTION("unbind_exchange")
		{
			channel.declare_exchange("exchange_bind_dest");
			channel.declare_exchange("exchange_bind_src");

			channel.bind_exchange("exchange_bind_dest", "exchange_bind_src", "rk");
			channel.unbind_exchange("exchange_bind_dest", "exchange_bind_src", "rk");

			channel.delete_exchange("exchange_bind_dest");
			channel.delete_exchange("exchange_bind_src");
		}

		/*SECTION("unbind_exchange_badbinding")
		{
			REQUIRE_THROWS_AS(
				channel.unbind_exchange("exchange_notexist", "exchange_notexist", "notexist"),
				channel_exception);
		}*/
	}
}
