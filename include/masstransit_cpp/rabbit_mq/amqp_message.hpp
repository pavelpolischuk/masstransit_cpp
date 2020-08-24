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
#include <masstransit_cpp/rabbit_mq/amqp_table.hpp>

#include <boost/noncopyable.hpp>

#include <string>
#include <memory>

struct amqp_bytes_t_;
struct amqp_basic_properties_t_;

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_message_impl;

		class MASSTRANSIT_CPP_API amqp_message : boost::noncopyable  // NOLINT(hicpp-special-member-functions)
		{
		public:
			enum delivery_modes { dm_non_persistent = 1, dm_persistent = 2 };

			amqp_message();
			explicit amqp_message(std::string const& body);
			amqp_message(amqp_bytes_t_ const& body, const amqp_basic_properties_t_* properties);

			virtual ~amqp_message();

			const amqp_basic_properties_t_* get_amqp_properties() const;
			const amqp_bytes_t_& get_amqp_body() const;
			
			std::string body() const;
			void set_body(std::string const& body);

			std::string content_type() const;
			void set_content_type(std::string const& content_type);
			bool has_content_type() const;
			void clear_content_type();
			
			std::string content_encoding() const;
			void set_content_encoding(std::string const& content_encoding);
			bool has_content_encoding() const;
			void clear_content_encoding();
			
			delivery_modes delivery_mode() const;
			void set_delivery_mode(delivery_modes delivery_mode);
			bool has_delivery_mode() const;
			void clear_delivery_mode();

			uint8_t priority() const;
			void set_priority(uint8_t priority);
			bool has_priority() const;
			void clear_priority();

			std::string correlation_id() const;
			void set_correlation_id(const std::string& correlation_id);
			bool has_correlation_id() const;
			void clear_correlation_id();

			std::string reply_to() const;
			void set_reply_to(const std::string& reply_to);
			bool has_reply_to() const;
			void clear_reply_to();

			std::string expiration() const;
			void set_expiration(const std::string& expiration);
			bool has_expiration() const;
			void clear_expiration();

			std::string message_id() const;
			void set_message_id(const std::string& message_id);
			bool has_message_id() const;
			void clear_message_id();

			uint64_t timestamp() const;
			void set_timestamp(uint64_t timestamp);
			bool has_timestamp() const;
			void clear_timestamp();

			std::string type() const;
			void set_type(const std::string& type);
			bool has_type() const;
			void clear_type();

			std::string user_id() const;
			void set_user_id(const std::string& user_id);
			bool has_user_id() const;
			void clear_user_id();

			std::string app_id() const;
			void set_app_id(const std::string& app_id);
			bool has_app_id() const;
			void clear_app_id();

			std::string cluster_id() const;
			void set_cluster_id(const std::string& cluster_id);
			bool has_cluster_id() const;
			void clear_cluster_id();

			amqp_table header_table() const;
			void set_header_table(const amqp_table& header_table);
			bool has_header_table() const;
			void clear_header_table();

		protected:
			std::unique_ptr<amqp_message_impl> impl_;
		};
	}
}
