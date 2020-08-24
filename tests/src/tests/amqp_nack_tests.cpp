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

	TEST_CASE("amqp_nack")
	{
		const auto host = masstransit_cpp::amqp_host("localhost", "guest", "guest");
		const auto channel = amqp_channel(host);

		SECTION("basic_nack_envelope")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message);

			const auto consumer = channel.consume(queue, "", true, false);

			std::shared_ptr<amqp_envelope> env;
			channel.consume_message(consumer, env);
			channel.reject(env->get_delivery_info(), false);
		}

		SECTION("basic_nack_deliveryinfo")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message);

			const auto consumer = channel.consume(queue, "", true, false);

			amqp_envelope::delivery_info info{};
			{
				std::shared_ptr<amqp_envelope> env;
				channel.consume_message(consumer, env);
				info = env->get_delivery_info();
			}

			channel.reject(info, false);
		}

		SECTION("basic_nack_envelope_with_requeue")
		{
			const auto message = std::make_shared<amqp_message>("Message Body");
			const auto queue = channel.declare_queue("");
			channel.publish("", queue, message);

			const auto consumer = channel.consume(queue, "", true, false);

			std::shared_ptr<amqp_envelope> env;
			channel.consume_message(consumer, env);

			channel.reject(env->get_delivery_info(), true);

			std::shared_ptr<amqp_envelope> env2;
			channel.consume_message(consumer, env2);

			channel.reject(env2->get_delivery_info(), false);
		}
	}
}
