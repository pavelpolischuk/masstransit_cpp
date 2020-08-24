#pragma once

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

#include <masstransit_cpp/global.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <memory>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_message;

		class MASSTRANSIT_CPP_API amqp_envelope : boost::noncopyable  // NOLINT(hicpp-special-member-functions, cppcoreguidelines-special-member-functions)
		{
		public:
			amqp_envelope(std::shared_ptr<amqp_message> message, std::string consumer_tag, uint64_t delivery_tag,
			              std::string exchange, bool redelivered, std::string routing_key, uint16_t delivery_channel);

			virtual ~amqp_envelope();

			inline std::shared_ptr<amqp_message> message() const { return message_; }

			inline std::string consumer_tag() const { return consumer_tag_; }

			inline uint64_t delivery_tag() const { return delivery_tag_; }

			inline std::string exchange() const { return exchange_; }

			inline bool redelivered() const { return redelivered_; }

			inline std::string routing_key() const { return routing_key_; }

			inline uint16_t delivery_channel() const { return delivery_channel_; }

			struct delivery_info
			{
				uint64_t delivery_tag;
				uint16_t delivery_channel;
			};

			inline delivery_info get_delivery_info() const
			{
				return {delivery_tag_, delivery_channel_};
			}

		private:
			const std::shared_ptr<amqp_message> message_;
			const std::string consumer_tag_;
			const uint64_t delivery_tag_;
			const std::string exchange_;
			const bool redelivered_;
			const std::string routing_key_;
			const uint16_t delivery_channel_;
		};
	}
}
