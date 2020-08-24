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

#include "masstransit_cpp/rabbit_mq/exceptions.hpp"

#include <sstream>
#include <cassert>
#include <utility>

namespace masstransit_cpp::rabbit_mq
{
	const uint16_t content_too_large_exception::REPLY_CODE = AMQP_CONTENT_TOO_LARGE;
	const uint16_t no_route_exception::REPLY_CODE = AMQP_NO_ROUTE;
	const uint16_t no_consumers_exception::REPLY_CODE = AMQP_NO_CONSUMERS;
	const uint16_t access_refused_exception::REPLY_CODE = AMQP_ACCESS_REFUSED;
	const uint16_t not_found_exception::REPLY_CODE = AMQP_NOT_FOUND;
	const uint16_t resource_locked_exception::REPLY_CODE = AMQP_RESOURCE_LOCKED;
	const uint16_t precondition_failed_exception::REPLY_CODE = AMQP_PRECONDITION_FAILED;
	const uint16_t connection_forced_exception::REPLY_CODE = AMQP_CONNECTION_FORCED;
	const uint16_t invalid_path_exception::REPLY_CODE = AMQP_INVALID_PATH;
	const uint16_t frame_error_exception::REPLY_CODE = AMQP_FRAME_ERROR;
	const uint16_t syntax_error_exception::REPLY_CODE = AMQP_SYNTAX_ERROR;
	const uint16_t command_invalid_exception::REPLY_CODE = AMQP_COMMAND_INVALID;
	const uint16_t channel_error_exception::REPLY_CODE = AMQP_CHANNEL_ERROR;
	const uint16_t unexpected_frame_exception::REPLY_CODE = AMQP_UNEXPECTED_FRAME;
	const uint16_t resource_error_exception::REPLY_CODE = AMQP_RESOURCE_ERROR;
	const uint16_t not_allowed_exception::REPLY_CODE = AMQP_NOT_ALLOWED;
	const uint16_t not_implemented_exception::REPLY_CODE = AMQP_NOT_IMPLEMENTED;
	const uint16_t internal_error_exception::REPLY_CODE = AMQP_INTERNAL_ERROR;

	void amqp_exception::throw_with(const amqp_rpc_reply_t& reply)
	{
		assert(reply.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION);

		switch (reply.reply.id)
		{
		case AMQP_CONNECTION_CLOSE_METHOD:
			throw_with(
				*(reinterpret_cast<amqp_connection_close_t *>(reply.reply.decoded)));
			break;
		case AMQP_CHANNEL_CLOSE_METHOD:
			throw_with(*(reinterpret_cast<amqp_channel_close_t *>(reply.reply.decoded)));
			break;
		default:
			throw std::logic_error(
				std::string("Programming error: unknown server exception class/method")
					.append(std::to_string(reply.reply.id)));
		}
	}

	void amqp_exception::throw_with(const amqp_channel_close_t& reply)
	{
		std::ostringstream what;

		std::string reply_text;
		if (reply.reply_text.bytes != nullptr)
		{
			reply_text =
				std::string(static_cast<char *>(reply.reply_text.bytes), reply.reply_text.len);
		}

		const auto method_name =
			amqp_method_name(((reply.class_id << 16) | reply.method_id));
		if (method_name != nullptr)
		{
			what << "channel error: " << reply.reply_code << ": " << method_name
				<< " caused: " << reply_text;
		}
		else
		{
			what << "channel error: " << reply.reply_code << ": " << reply_text;
		}

		switch (reply.reply_code)
		{
		case content_too_large_exception::REPLY_CODE:
			throw content_too_large_exception(what.str(), reply_text, reply.class_id,
			                               reply.method_id);
		case no_route_exception::REPLY_CODE:
			throw no_route_exception(what.str(), reply_text, reply.class_id,
			                       reply.method_id);
		case no_consumers_exception::REPLY_CODE:
			throw no_consumers_exception(what.str(), reply_text, reply.class_id,
			                           reply.method_id);
		case access_refused_exception::REPLY_CODE:
			throw access_refused_exception(what.str(), reply_text, reply.class_id,
			                             reply.method_id);
		case not_found_exception::REPLY_CODE:
			throw not_found_exception(what.str(), reply_text, reply.class_id,
			                        reply.method_id);
		case resource_locked_exception::REPLY_CODE:
			throw resource_locked_exception(what.str(), reply_text, reply.class_id,
			                              reply.method_id);
		case precondition_failed_exception::REPLY_CODE:
			throw precondition_failed_exception(what.str(), reply_text, reply.class_id,
			                                  reply.method_id);
		default:
			throw std::logic_error(
				std::string("Programming error: unknown channel reply code: ")
				.append(std::to_string(reply.reply_code)));
		}
	}

	void amqp_exception::throw_with(const amqp_connection_close_t& reply)
	{
		const auto method_name = amqp_method_name(((reply.class_id << 16) | reply.method_id));

		std::string reply_text;
		if (reply.reply_text.bytes != nullptr)
		{
			reply_text =
				std::string(static_cast<char *>(reply.reply_text.bytes), reply.reply_text.len);
		}

		std::ostringstream what;
		if (method_name != nullptr)
		{
			what << "connection error: " << reply.reply_code << ": " << method_name
				<< " caused: " << reply_text;
		}
		else
		{
			what << "connection error: " << reply.reply_code << ": " << reply_text;
		}

		switch (reply.reply_code)
		{
		case connection_forced_exception::REPLY_CODE:
			throw connection_forced_exception(what.str(), reply_text, reply.class_id,
			                                reply.method_id);
		case invalid_path_exception::REPLY_CODE:
			throw invalid_path_exception(what.str(), reply_text, reply.class_id,
			                           reply.method_id);
		case frame_error_exception::REPLY_CODE:
			throw frame_error_exception(what.str(), reply_text, reply.class_id,
			                          reply.method_id);
		case syntax_error_exception::REPLY_CODE:
			throw syntax_error_exception(what.str(), reply_text, reply.class_id,
			                           reply.method_id);
		case command_invalid_exception::REPLY_CODE:
			throw command_invalid_exception(what.str(), reply_text, reply.class_id,
			                              reply.method_id);
		case channel_error_exception::REPLY_CODE:
			throw channel_error_exception(what.str(), reply_text, reply.class_id,
			                            reply.method_id);
		case unexpected_frame_exception::REPLY_CODE:
			throw unexpected_frame_exception(what.str(), reply_text, reply.class_id,
			                               reply.method_id);
		case resource_error_exception::REPLY_CODE:
			throw resource_error_exception(what.str(), reply_text, reply.class_id,
			                             reply.method_id);
		case not_allowed_exception::REPLY_CODE:
			throw not_allowed_exception(what.str(), reply_text, reply.class_id,
			                          reply.method_id);
		case not_implemented_exception::REPLY_CODE:
			throw not_implemented_exception(what.str(), reply_text, reply.class_id,
			                              reply.method_id);
		case internal_error_exception::REPLY_CODE:
			throw internal_error_exception(what.str(), reply_text, reply.class_id,
			                             reply.method_id);
		case access_refused_exception::REPLY_CODE:
			throw access_refused_exception(what.str(), reply_text, reply.class_id,
			                             reply.method_id);
		default:
			throw std::logic_error(
				std::string("Programming error: unknown connection reply code: ")
				.append(std::to_string(reply.reply_code)));
		}
	}

	amqp_exception::amqp_exception(std::string const& what, std::string reply_text,
	                               const uint16_t class_id, const uint16_t method_id) noexcept
		: std::runtime_error(what)
		, reply_text_(std::move(reply_text))
		, class_id_(class_id)
		, method_id_(method_id)
	{
	}

	amqp_exception::~amqp_exception() noexcept = default;

	uint16_t amqp_exception::class_id() const noexcept
	{
		return class_id_;
	}

	uint16_t amqp_exception::method_id() const noexcept
	{
		return method_id_;
	}

	std::string amqp_exception::reply_text() const noexcept
	{
		return reply_text_;
	}


	connection_exception::connection_exception(std::string const& what, std::string const& reply_text,
	                                           const uint16_t class_id, const uint16_t method_id) noexcept
		: amqp_exception(what, reply_text, class_id, method_id)
	{
	}

	bool connection_exception::is_soft_error() const noexcept
	{
		return false;
	}

	channel_exception::channel_exception(std::string const& what, std::string const& reply_text,
	                                     const uint16_t class_id, const uint16_t method_id) noexcept
		: amqp_exception(what, reply_text, class_id, method_id)
	{
	}

	bool channel_exception::is_soft_error() const noexcept
	{
		return true;
	}

	connection_forced_exception::connection_forced_exception(std::string const& what, std::string const& reply_text,
	                                                         const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t connection_forced_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	invalid_path_exception::invalid_path_exception(std::string const& what, std::string const& reply_text,
	                                               const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t invalid_path_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	frame_error_exception::frame_error_exception(std::string const& what, std::string const& reply_text,
	                                             const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t frame_error_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	syntax_error_exception::syntax_error_exception(std::string const& what, std::string const& reply_text,
	                                               const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t syntax_error_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	command_invalid_exception::command_invalid_exception(std::string const& what, std::string const& reply_text,
	                                                     const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t command_invalid_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	channel_error_exception::channel_error_exception(std::string const& what, std::string const& reply_text,
	                                                 const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t channel_error_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	unexpected_frame_exception::unexpected_frame_exception(std::string const& what, std::string const& reply_text,
	                                                       const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t unexpected_frame_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	resource_error_exception::resource_error_exception(std::string const& what, std::string const& reply_text,
	                                                   const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t resource_error_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	not_allowed_exception::not_allowed_exception(std::string const& what, std::string const& reply_text,
	                                             const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t not_allowed_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	not_implemented_exception::not_implemented_exception(std::string const& what, std::string const& reply_text,
	                                                     const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t not_implemented_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	internal_error_exception::internal_error_exception(std::string const& what, std::string const& reply_text,
	                                                   const uint16_t class_id, const uint16_t method_id) noexcept
		: connection_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t internal_error_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	content_too_large_exception::content_too_large_exception(std::string const& what, std::string const& reply_text,
	                                                         const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t content_too_large_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	no_route_exception::no_route_exception(std::string const& what, std::string const& reply_text,
	                                       const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t no_route_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	no_consumers_exception::no_consumers_exception(std::string const& what, std::string const& reply_text,
	                                               const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t no_consumers_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	access_refused_exception::access_refused_exception(std::string const& what, std::string const& reply_text,
	                                                   const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t access_refused_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	not_found_exception::not_found_exception(std::string const& what, std::string const& reply_text,
	                                         const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t not_found_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	resource_locked_exception::resource_locked_exception(std::string const& what, std::string const& reply_text,
	                                                     const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t resource_locked_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	precondition_failed_exception::precondition_failed_exception(std::string const& what, std::string const& reply_text,
	                                                             const uint16_t class_id, const uint16_t method_id) noexcept
		: channel_exception(what, reply_text, class_id, method_id)
	{
	}

	uint16_t precondition_failed_exception::reply_code() const noexcept
	{
		return REPLY_CODE;
	}

	bad_uri_exception::bad_uri_exception()
		: std::runtime_error("URI is malformed")
	{
	}

	amqp_library_exception amqp_library_exception::create_exception(const int error_code)
	{
		const std::string message(amqp_error_string2(error_code));

		return amqp_library_exception(message, error_code);
	}

	amqp_library_exception amqp_library_exception::create_exception(
		const int error_code, std::string const& context)
	{
		auto message(context);
		message.append(": ");
		message.append(amqp_error_string2(error_code));

		return amqp_library_exception(message, error_code);
	}

	int amqp_library_exception::error_code() const
	{
		return error_code_;
	}

	amqp_library_exception::amqp_library_exception(std::string const& message, const int error_code) noexcept
		: std::runtime_error(message)
		, error_code_(error_code)
	{
	}


	amqp_response_library_exception amqp_response_library_exception::create_exception(
		amqp_rpc_reply_t_ const& reply, std::string const& context)
	{
		auto message(context);
		message.append(": ");
		message.append(amqp_error_string2(reply.library_error));

		return amqp_response_library_exception(message);
	}

	amqp_response_library_exception::amqp_response_library_exception(std::string const& message) noexcept
		: std::runtime_error(message)
	{
	}

	connection_closed_exception::connection_closed_exception()
		: std::runtime_error("Connection is closed")
	{
	}

	consumer_cancelled_exception::consumer_cancelled_exception(std::string const& consumer_tag) noexcept
		: std::runtime_error(std::string("Consumer was cancelled: ").append(consumer_tag))
		, consumer_tag_(consumer_tag)
	{
	}

	consumer_cancelled_exception::~consumer_cancelled_exception() noexcept = default;

	std::string consumer_cancelled_exception::get_consumer_tag() const
	{
		return consumer_tag_;
	}

	consumer_tag_not_found_exception::consumer_tag_not_found_exception() noexcept
		: std::runtime_error("The specified consumer tag is unknown")
	{
	}

	consumer_tag_not_found_exception::~consumer_tag_not_found_exception() noexcept = default;


	message_returned_exception::message_returned_exception(const std::shared_ptr<rabbit_mq::amqp_message> message, 
		const uint32_t reply_code, std::string const& reply_text,
		std::string const& exchange, std::string const& routing_key) noexcept
		: std::runtime_error(std::string("Message returned. Reply code: ")
			  .append(std::to_string(reply_code))
			  .append(" ")
			  .append(reply_text)),
		  message_(message),
		  reply_code_(reply_code),
		  reply_text_(reply_text),
		  exchange_(exchange),
		  routing_key_(routing_key)
	{
	}

	message_returned_exception::~message_returned_exception() noexcept = default;

	std::shared_ptr<rabbit_mq::amqp_message> message_returned_exception::message() const noexcept
	{
		return message_;
	}

	uint32_t message_returned_exception::reply_code() const noexcept
	{
		return reply_code_;
	}

	std::string message_returned_exception::reply_text() const noexcept
	{
		return reply_text_;
	}

	std::string message_returned_exception::exchange() const noexcept
	{
		return exchange_;
	}

	std::string message_returned_exception::routing_key() const noexcept
	{
		return routing_key_;
	}
}
	  