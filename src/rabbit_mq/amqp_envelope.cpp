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

#include "masstransit_cpp/rabbit_mq/amqp_envelope.hpp"

namespace masstransit_cpp::rabbit_mq
{
	amqp_envelope::amqp_envelope(std::shared_ptr<amqp_message> message, 
		std::string consumer_tag, const uint64_t delivery_tag, 
		std::string exchange, const bool redelivered, 
		std::string routing_key, const uint16_t delivery_channel)
		: message_(std::move(message))
		, consumer_tag_(std::move(consumer_tag))
		, delivery_tag_(delivery_tag)
		, exchange_(std::move(exchange))
		, redelivered_(redelivered)
		, routing_key_(std::move(routing_key))
		, delivery_channel_(delivery_channel)
	{
	}

	amqp_envelope::~amqp_envelope() = default;
}
