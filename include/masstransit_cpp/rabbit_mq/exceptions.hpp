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
#include <stdexcept>
#include <string>
#include <memory>

struct amqp_rpc_reply_t_;
struct amqp_channel_close_t_;
struct amqp_connection_close_t_;

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class MASSTRANSIT_CPP_API amqp_exception : public std::runtime_error  // NOLINT(cppcoreguidelines-special-member-functions)
		{
		public:
			static void throw_with(const amqp_rpc_reply_t_& reply);
			static void throw_with(const amqp_channel_close_t_& reply);
			static void throw_with(const amqp_connection_close_t_& reply);

			amqp_exception(std::string const& what, std::string reply_text,
			               uint16_t class_id, uint16_t method_id) noexcept;

			virtual ~amqp_exception() noexcept;

			virtual bool is_soft_error() const noexcept = 0;
			virtual uint16_t reply_code() const noexcept = 0;
			virtual uint16_t class_id() const noexcept;
			virtual uint16_t method_id() const noexcept;
			virtual std::string reply_text() const noexcept;

		protected:
			std::string reply_text_;
			uint16_t class_id_;
			uint16_t method_id_;
		};

		class connection_exception : public amqp_exception
		{
		public:
			connection_exception(std::string const& what, std::string const& reply_text,
			                     uint16_t class_id, uint16_t method_id) noexcept;

			bool is_soft_error() const noexcept override;
		};

		class channel_exception : public amqp_exception
		{
		public:
			channel_exception(std::string const& what, std::string const& reply_text,
			                  uint16_t class_id, uint16_t method_id) noexcept;

			bool is_soft_error() const noexcept override;
		};

		class connection_forced_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			connection_forced_exception(std::string const& what, std::string const& reply_text,
			                            uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class invalid_path_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			invalid_path_exception(std::string const& what, std::string const& reply_text,
			                       uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class frame_error_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			frame_error_exception(std::string const& what, std::string const& reply_text,
			                      uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class syntax_error_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			syntax_error_exception(std::string const& what, std::string const& reply_text,
			                       uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class command_invalid_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			command_invalid_exception(std::string const& what, std::string const& reply_text,
			                          uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class channel_error_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			channel_error_exception(std::string const& what, std::string const& reply_text,
			                        uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class unexpected_frame_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			unexpected_frame_exception(std::string const& what, std::string const& reply_text,
			                           uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class resource_error_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			resource_error_exception(std::string const& what, std::string const& reply_text,
			                         uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class not_allowed_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			not_allowed_exception(std::string const& what, std::string const& reply_text,
			                      uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class not_implemented_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			not_implemented_exception(std::string const& what, std::string const& reply_text,
			                          uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class internal_error_exception : public connection_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			internal_error_exception(std::string const& what, std::string const& reply_text,
			                         uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class content_too_large_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			content_too_large_exception(std::string const& what, std::string const& reply_text,
			                                     uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class no_route_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			no_route_exception(std::string const& what, std::string const& reply_text,
			                   uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class no_consumers_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			no_consumers_exception(std::string const& what, std::string const& reply_text,
			                       uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class access_refused_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			access_refused_exception(std::string const& what, std::string const& reply_text,
			                         uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class not_found_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			not_found_exception(std::string const& what, std::string const& reply_text,
			                    uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class resource_locked_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			resource_locked_exception(std::string const& what, std::string const& reply_text,
			                          uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};

		class precondition_failed_exception : public channel_exception
		{
		public:
			static const uint16_t REPLY_CODE;

			precondition_failed_exception(std::string const& what, std::string const& reply_text,
			                              uint16_t class_id, uint16_t method_id) noexcept;

			uint16_t reply_code() const noexcept override;
		};


		class bad_uri_exception : public std::runtime_error
		{
		public:
			bad_uri_exception();
		};

		class amqp_library_exception : public std::runtime_error
		{
		public:
			static amqp_library_exception create_exception(int error_code);
			static amqp_library_exception create_exception(int error_code, std::string const& context);

			int error_code() const;

		protected:
			amqp_library_exception(std::string const& message, int error_code) noexcept;

		private:
			int error_code_;
		};


		class amqp_response_library_exception : public std::runtime_error
		{
		public:
			static amqp_response_library_exception create_exception(amqp_rpc_reply_t_ const& reply, std::string const& context);

		protected:
			explicit amqp_response_library_exception(std::string const& message) noexcept;
		};


		class connection_closed_exception : public std::runtime_error
		{
		public:
			connection_closed_exception();
		};

		class consumer_cancelled_exception : public std::runtime_error  // NOLINT
		{
		public:
			explicit consumer_cancelled_exception(std::string const& consumer_tag) noexcept;

			virtual ~consumer_cancelled_exception() noexcept;

			std::string get_consumer_tag() const;

		private:
			std::string consumer_tag_;
		};


		class consumer_tag_not_found_exception : public std::runtime_error  // NOLINT
		{
		public:
			consumer_tag_not_found_exception() noexcept;

			virtual ~consumer_tag_not_found_exception() noexcept;
		};

		class amqp_message;

		class message_returned_exception : public std::runtime_error  // NOLINT
		{
		public:
			message_returned_exception(std::shared_ptr<rabbit_mq::amqp_message> message,
			                           uint32_t reply_code, std::string const& reply_text,
			                           std::string const& exchange, std::string const& routing_key) noexcept;

			virtual ~message_returned_exception() noexcept;

			std::shared_ptr<rabbit_mq::amqp_message> message() const noexcept;
			uint32_t reply_code() const noexcept;
			std::string reply_text() const noexcept;
			std::string exchange() const noexcept;
			std::string routing_key() const noexcept;

		private:
			std::shared_ptr<rabbit_mq::amqp_message> message_;
			uint32_t reply_code_;
			std::string reply_text_;
			std::string exchange_;
			std::string routing_key_;
			mutable std::string what_;
		};
	}
}
