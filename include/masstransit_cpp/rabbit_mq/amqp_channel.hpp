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
#include <masstransit_cpp/rabbit_mq/amqp_host.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_message.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_envelope.hpp>
#include <masstransit_cpp/rabbit_mq/amqp_table.hpp>

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <any>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_channel_impl;

		class MASSTRANSIT_CPP_API amqp_channel : boost::noncopyable  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
		{
		public:
			static inline const char* EXCHANGE_TYPE_DIRECT{ "direct" };
			static inline const char* EXCHANGE_TYPE_FANOUT{ "fanout" };
			static inline const char* EXCHANGE_TYPE_TOPIC{ "topic" };

			explicit amqp_channel(amqp_host const& host, int heartbeat = 10, int frame_max = 131072);
			virtual ~amqp_channel();

			void declare_exchange(std::string const& exchange_name, std::string const& exchange_type = EXCHANGE_TYPE_DIRECT,
			                      bool passive = false, bool durable = false, bool auto_delete = false, amqp_table const& args = {}) const;

			void delete_exchange(std::string const& exchange_name, bool if_unused = false) const;
			void bind_exchange(std::string const& destination, std::string const& source,
			                   std::string const& routing_key, amqp_table const& args = {}) const;
			void unbind_exchange(std::string const& destination, std::string const& source,
			                     std::string const& routing_key, amqp_table const& args = {}) const;

			std::string declare_queue(std::string const& queue_name, bool passive = false, bool durable = false,
			                          bool exclusive = true, bool auto_delete = true, amqp_table const& args = {}) const;

			std::string declare_queue_with_counts(std::string const& queue_name, uint32_t& message_count, uint32_t& consumer_count, bool passive = false,
			                                      bool durable = false, bool exclusive = true, bool auto_delete = true, amqp_table const& args = {}) const;

			void delete_queue(std::string const& queue_name, bool if_unused = false, bool if_empty = false) const;
			void purge_queue(std::string const& queue_name) const;

			void bind_queue(std::string const& queue_name, std::string const& exchange_name,
			                std::string const& routing_key = "", amqp_table const& args = {}) const;

			void unbind_queue(std::string const& queue_name, std::string const& exchange_name,
			                  std::string const& routing_key = "", amqp_table const& args = {}) const;

			void ack(amqp_envelope::delivery_info const& info) const;
			void reject(amqp_envelope::delivery_info const& info, bool requeue, bool multiple = false) const;

			void publish(std::string const& exchange_name, std::string const& routing_key, std::shared_ptr<amqp_message> const& message, 
						 bool mandatory = false, bool immediate = false) const;

			bool get(std::shared_ptr<amqp_envelope>& envelope, std::string const& queue, bool no_ack = true) const;

			void recover(std::string const& consumer) const;

			std::string consume(std::string const& queue, std::string const& consumer_tag = "", bool no_local = true,
			                    bool no_ack = true, bool exclusive = true, uint16_t prefetch_count = 1, amqp_table const& args = {}) const;

			void qos(std::string const& consumer_tag, uint16_t prefetch_count) const;

			void cancel(std::string const& consumer_tag) const;

			bool consume_message(std::string const& consumer_tag, std::shared_ptr<amqp_envelope>& envelope, std::chrono::microseconds timeout = std::chrono::microseconds::max()) const;
			bool consume_message(std::vector<std::string> const& consumer_tags, std::shared_ptr<amqp_envelope>& envelope, std::chrono::microseconds timeout = std::chrono::microseconds::max()) const;
			bool consume_message(std::shared_ptr<amqp_envelope>& envelope, std::chrono::microseconds timeout = std::chrono::microseconds::max()) const;

		private:
			std::unique_ptr<amqp_channel_impl> impl_;
		};
	}
}
