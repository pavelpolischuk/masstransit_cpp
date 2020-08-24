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

#include <amqp.h>
#include <amqp_framing.h>

#include "masstransit_cpp/rabbit_mq/amqp_message.hpp"

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class amqp_message_impl
		{
		public:
			amqp_message_impl()
				: properties()
				, body()
			{
			}

			amqp_basic_properties_t properties;
			amqp_bytes_t body;
			std::shared_ptr<amqp_pool_t> table_pool;
		};

		amqp_message::amqp_message()
			: impl_(new amqp_message_impl)
		{
			impl_->body.bytes = nullptr;
			impl_->body.len = 0;
			impl_->properties._flags = 0;
		}

		amqp_message::amqp_message(std::string const& body)
			: impl_(new amqp_message_impl)
		{
			set_body(body);
			impl_->properties._flags = 0;
		}

		amqp_message::amqp_message(amqp_bytes_t const& body, const amqp_basic_properties_t* properties)
			: impl_(new amqp_message_impl)
		{
			impl_->body = body;
			impl_->properties = *properties;
			if (has_content_type())
				impl_->properties.content_type =
					amqp_bytes_malloc_dup(impl_->properties.content_type);
			if (has_content_encoding())
				impl_->properties.content_encoding =
					amqp_bytes_malloc_dup(impl_->properties.content_encoding);
			if (has_correlation_id())
				impl_->properties.correlation_id =
					amqp_bytes_malloc_dup(impl_->properties.correlation_id);
			if (has_reply_to())
				impl_->properties.reply_to =
					amqp_bytes_malloc_dup(impl_->properties.reply_to);
			if (has_expiration())
				impl_->properties.expiration =
					amqp_bytes_malloc_dup(impl_->properties.expiration);
			if (has_message_id())
				impl_->properties.message_id =
					amqp_bytes_malloc_dup(impl_->properties.message_id);
			if (has_type())
				impl_->properties.type =
					amqp_bytes_malloc_dup(impl_->properties.type);
			if (has_user_id())
				impl_->properties.user_id =
					amqp_bytes_malloc_dup(impl_->properties.user_id);
			if (has_app_id())
				impl_->properties.app_id =
					amqp_bytes_malloc_dup(impl_->properties.app_id);
			if (has_cluster_id())
				impl_->properties.cluster_id =
					amqp_bytes_malloc_dup(impl_->properties.cluster_id);
			if (has_header_table())
				impl_->properties.headers = amqp_table::copy_amqp(
					impl_->properties.headers, impl_->table_pool);
		}

		amqp_message::~amqp_message()
		{
			amqp_bytes_free(impl_->body);
			if (has_content_type()) amqp_bytes_free(impl_->properties.content_type);
			if (has_content_encoding())
				amqp_bytes_free(impl_->properties.content_encoding);
			if (has_correlation_id())
				amqp_bytes_free(impl_->properties.correlation_id);
			if (has_reply_to()) amqp_bytes_free(impl_->properties.reply_to);
			if (has_expiration()) amqp_bytes_free(impl_->properties.expiration);
			if (has_message_id()) amqp_bytes_free(impl_->properties.message_id);
			if (has_type()) amqp_bytes_free(impl_->properties.type);
			if (has_user_id()) amqp_bytes_free(impl_->properties.user_id);
			if (has_app_id()) amqp_bytes_free(impl_->properties.app_id);
			if (has_cluster_id()) amqp_bytes_free(impl_->properties.cluster_id);
		}

		const amqp_basic_properties_t* amqp_message::get_amqp_properties() const
		{
			return &impl_->properties;
		}

		const amqp_bytes_t& amqp_message::get_amqp_body() const { return impl_->body; }

		std::string amqp_message::body() const
		{
			if (impl_->body.bytes == nullptr)
				return std::string();
			else
				return std::string(static_cast<char *>(impl_->body.bytes), impl_->body.len);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_body(const std::string& body)
		{
			if (nullptr != impl_->body.bytes)
			{
				amqp_bytes_free(impl_->body);
			}
			amqp_bytes_t body_bytes;
			body_bytes.bytes = const_cast<char *>(body.data());
			body_bytes.len = body.length();
			impl_->body = amqp_bytes_malloc_dup(body_bytes);
		}

		std::string amqp_message::content_type() const
		{
			if (has_content_type())
				return std::string(static_cast<char *>(impl_->properties.content_type.bytes),
				                   impl_->properties.content_type.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_content_type(const std::string& content_type)
		{
			if (has_content_type()) amqp_bytes_free(impl_->properties.content_type);
			impl_->properties.content_type =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(content_type.c_str()));
			impl_->properties._flags |= AMQP_BASIC_CONTENT_TYPE_FLAG;
		}

		bool amqp_message::has_content_type() const
		{
			return AMQP_BASIC_CONTENT_TYPE_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_content_type()
		{
			if (has_content_type()) amqp_bytes_free(impl_->properties.content_type);
			impl_->properties._flags &= ~AMQP_BASIC_CONTENT_TYPE_FLAG;
		}

		std::string amqp_message::content_encoding() const
		{
			if (has_content_encoding())
				return std::string(static_cast<char *>(impl_->properties.content_encoding.bytes),
				                   impl_->properties.content_encoding.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_content_encoding(const std::string& content_encoding)
		{
			if (has_content_encoding())
				amqp_bytes_free(impl_->properties.content_encoding);
			impl_->properties.content_encoding =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(content_encoding.c_str()));
			impl_->properties._flags |= AMQP_BASIC_CONTENT_ENCODING_FLAG;
		}

		bool amqp_message::has_content_encoding() const
		{
			return AMQP_BASIC_CONTENT_ENCODING_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_CONTENT_ENCODING_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_content_encoding()
		{
			if (has_content_encoding())
				amqp_bytes_free(impl_->properties.content_encoding);
			impl_->properties._flags &= ~AMQP_BASIC_CONTENT_ENCODING_FLAG;
		}

		amqp_message::delivery_modes amqp_message::delivery_mode() const
		{
			if (has_delivery_mode())
				return delivery_modes(impl_->properties.delivery_mode);
			else
				return delivery_modes(0);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_delivery_mode(delivery_modes delivery_mode)
		{
			impl_->properties.delivery_mode = static_cast<uint8_t>(delivery_mode);
			impl_->properties._flags |= AMQP_BASIC_DELIVERY_MODE_FLAG;
		}

		bool amqp_message::has_delivery_mode() const
		{
			return AMQP_BASIC_DELIVERY_MODE_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_DELIVERY_MODE_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_delivery_mode()
		{
			impl_->properties._flags &= ~AMQP_BASIC_DELIVERY_MODE_FLAG;
		}

		uint8_t amqp_message::priority() const
		{
			if (has_priority())
				return impl_->properties.priority;
			else
				return 0;
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_priority(uint8_t priority)
		{
			impl_->properties.priority = priority;
			impl_->properties._flags |= AMQP_BASIC_PRIORITY_FLAG;
		}

		bool amqp_message::has_priority() const
		{
			return AMQP_BASIC_PRIORITY_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_PRIORITY_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_priority()
		{
			impl_->properties._flags &= ~AMQP_BASIC_PRIORITY_FLAG;
		}

		std::string amqp_message::correlation_id() const
		{
			if (has_correlation_id())
				return std::string(static_cast<char *>(impl_->properties.correlation_id.bytes),
				                   impl_->properties.correlation_id.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_correlation_id(const std::string& correlation_id)
		{
			if (has_correlation_id())
				amqp_bytes_free(impl_->properties.correlation_id);
			impl_->properties.correlation_id =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(correlation_id.c_str()));
			impl_->properties._flags |= AMQP_BASIC_CORRELATION_ID_FLAG;
		}

		bool amqp_message::has_correlation_id() const
		{
			return AMQP_BASIC_CORRELATION_ID_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_correlation_id()
		{
			if (has_correlation_id())
				amqp_bytes_free(impl_->properties.correlation_id);
			impl_->properties._flags &= ~AMQP_BASIC_CORRELATION_ID_FLAG;
		}

		std::string amqp_message::reply_to() const
		{
			if (has_reply_to())
				return std::string(static_cast<char *>(impl_->properties.reply_to.bytes),
				                   impl_->properties.reply_to.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_reply_to(const std::string& reply_to)
		{
			if (has_reply_to()) amqp_bytes_free(impl_->properties.reply_to);
			impl_->properties.reply_to =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(reply_to.c_str()));
			impl_->properties._flags |= AMQP_BASIC_REPLY_TO_FLAG;
		}

		bool amqp_message::has_reply_to() const
		{
			return AMQP_BASIC_REPLY_TO_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_REPLY_TO_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_reply_to()
		{
			if (has_reply_to()) amqp_bytes_free(impl_->properties.reply_to);
			impl_->properties._flags &= ~AMQP_BASIC_REPLY_TO_FLAG;
		}

		std::string amqp_message::expiration() const
		{
			if (has_expiration())
				return std::string(static_cast<char *>(impl_->properties.expiration.bytes),
				                   impl_->properties.expiration.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_expiration(const std::string& expiration)
		{
			if (has_expiration()) amqp_bytes_free(impl_->properties.expiration);
			impl_->properties.expiration =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(expiration.c_str()));
			impl_->properties._flags |= AMQP_BASIC_EXPIRATION_FLAG;
		}

		bool amqp_message::has_expiration() const
		{
			return AMQP_BASIC_EXPIRATION_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_EXPIRATION_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_expiration()
		{
			if (has_expiration()) amqp_bytes_free(impl_->properties.expiration);
			impl_->properties._flags &= ~AMQP_BASIC_EXPIRATION_FLAG;
		}

		std::string amqp_message::message_id() const
		{
			if (has_message_id())
				return std::string(static_cast<char *>(impl_->properties.message_id.bytes),
				                   impl_->properties.message_id.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_message_id(const std::string& message_id)
		{
			if (has_message_id()) amqp_bytes_free(impl_->properties.message_id);
			impl_->properties.message_id =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(message_id.c_str()));
			impl_->properties._flags |= AMQP_BASIC_MESSAGE_ID_FLAG;
		}

		bool amqp_message::has_message_id() const
		{
			return AMQP_BASIC_MESSAGE_ID_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_MESSAGE_ID_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_message_id()
		{
			if (has_message_id()) amqp_bytes_free(impl_->properties.message_id);
			impl_->properties._flags &= ~AMQP_BASIC_MESSAGE_ID_FLAG;
		}

		uint64_t amqp_message::timestamp() const
		{
			if (has_timestamp())
				return impl_->properties.timestamp;
			else
				return 0;
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_timestamp(uint64_t timestamp)
		{
			impl_->properties.timestamp = timestamp;
			impl_->properties._flags |= AMQP_BASIC_TIMESTAMP_FLAG;
		}

		bool amqp_message::has_timestamp() const
		{
			return AMQP_BASIC_TIMESTAMP_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_TIMESTAMP_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_timestamp()
		{
			impl_->properties._flags &= ~AMQP_BASIC_TIMESTAMP_FLAG;
		}

		std::string amqp_message::type() const
		{
			if (has_type())
				return std::string(static_cast<char *>(impl_->properties.type.bytes),
				                   impl_->properties.type.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_type(const std::string& type)
		{
			if (has_type()) amqp_bytes_free(impl_->properties.type);
			impl_->properties.type =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(type.c_str()));
			impl_->properties._flags |= AMQP_BASIC_TYPE_FLAG;
		}

		bool amqp_message::has_type() const
		{
			return AMQP_BASIC_TYPE_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_TYPE_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_type()
		{
			if (has_type()) amqp_bytes_free(impl_->properties.type);
			impl_->properties._flags &= ~AMQP_BASIC_TYPE_FLAG;
		}

		std::string amqp_message::user_id() const
		{
			if (has_user_id())
				return std::string(static_cast<char *>(impl_->properties.user_id.bytes),
				                   impl_->properties.user_id.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_user_id(const std::string& user_id)
		{
			if (has_user_id()) amqp_bytes_free(impl_->properties.user_id);
			impl_->properties.user_id =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(user_id.c_str()));
			impl_->properties._flags |= AMQP_BASIC_USER_ID_FLAG;
		}

		bool amqp_message::has_user_id() const
		{
			return AMQP_BASIC_USER_ID_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_USER_ID_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_user_id()
		{
			if (has_user_id()) amqp_bytes_free(impl_->properties.user_id);
			impl_->properties._flags &= ~AMQP_BASIC_USER_ID_FLAG;
		}

		std::string amqp_message::app_id() const
		{
			if (has_app_id())
				return std::string(static_cast<char *>(impl_->properties.app_id.bytes),
				                   impl_->properties.app_id.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_app_id(const std::string& app_id)
		{
			if (has_app_id()) amqp_bytes_free(impl_->properties.app_id);
			impl_->properties.app_id =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(app_id.c_str()));
			impl_->properties._flags |= AMQP_BASIC_APP_ID_FLAG;
		}

		bool amqp_message::has_app_id() const
		{
			return AMQP_BASIC_APP_ID_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_APP_ID_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_app_id()
		{
			if (has_app_id()) amqp_bytes_free(impl_->properties.app_id);
			impl_->properties._flags &= ~AMQP_BASIC_APP_ID_FLAG;
		}

		std::string amqp_message::cluster_id() const
		{
			if (has_cluster_id())
				return std::string(static_cast<char *>(impl_->properties.cluster_id.bytes),
				                   impl_->properties.cluster_id.len);
			else
				return std::string();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_cluster_id(const std::string& cluster_id)
		{
			if (has_cluster_id()) amqp_bytes_free(impl_->properties.cluster_id);
			impl_->properties.cluster_id =
				amqp_bytes_malloc_dup(amqp_cstring_bytes(cluster_id.c_str()));
			impl_->properties._flags |= AMQP_BASIC_CLUSTER_ID_FLAG;
		}

		bool amqp_message::has_cluster_id() const
		{
			return AMQP_BASIC_CLUSTER_ID_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_CLUSTER_ID_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_cluster_id()
		{
			if (has_cluster_id()) amqp_bytes_free(impl_->properties.cluster_id);
			impl_->properties._flags &= ~AMQP_BASIC_CLUSTER_ID_FLAG;
		}

		amqp_table amqp_message::header_table() const
		{
			if (has_header_table())
				return amqp_table(impl_->properties.headers);
			else
				return amqp_table();
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::set_header_table(const amqp_table& header_table)
		{
			impl_->properties.headers = header_table.to_amqp(impl_->table_pool);
			impl_->properties._flags |= AMQP_BASIC_HEADERS_FLAG;
		}

		bool amqp_message::has_header_table() const
		{
			return AMQP_BASIC_HEADERS_FLAG ==
				(impl_->properties._flags & AMQP_BASIC_HEADERS_FLAG);
		}

		// ReSharper disable once CppMemberFunctionMayBeConst
		void amqp_message::clear_header_table()
		{
			if (has_header_table())
			{
				impl_->table_pool.reset();
				impl_->properties.headers.num_entries = 0;
				impl_->properties.headers.entries = nullptr;
			}
			impl_->properties._flags &= ~AMQP_BASIC_HEADERS_FLAG;
		}
	}
}
