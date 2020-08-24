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
#include <amqp_tcp_socket.h>
#include <amqp_ssl_socket.h>

#ifdef _WIN32
#define NOMINMAX
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Winsock2.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#endif

#include "masstransit_cpp/rabbit_mq/amqp_channel.hpp"
#include "masstransit_cpp/rabbit_mq/exceptions.hpp"

#include <map>
#include <queue>
#include <stdexcept>
#include <utility>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		namespace
		{
			bool operator==(amqp_bytes_t const& r, amqp_bytes_t const& l)
			{
				if (r.len == l.len)
				{
					if (0 == memcmp(r.bytes, l.bytes, r.len))
					{
						return true;
					}
				}
				return false;
			}
		}

		class amqp_channel_impl : boost::noncopyable  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
		{
		public:
			amqp_channel_impl()
				: is_connected_(false)
				, last_used_channel_(0) 
			{
				channels_.push_back(cs_used);
			}
			
			virtual ~amqp_channel_impl() = default;

			typedef std::vector<amqp_channel_t> channel_list_t;
			typedef std::vector<amqp_frame_t> frame_queue_t;
			typedef std::map<amqp_channel_t, frame_queue_t> channel_map_t;
			typedef channel_map_t::iterator channel_map_iterator_t;

			enum channel_states { cs_closed = 0, cs_open, cs_used };

			void do_login(std::string const& username, std::string const& password, std::string const& vhost, const int heartbeat, const int frame_max)
			{
				amqp_table_entry_t capabilities[1];
				amqp_table_entry_t capability_entry;
				amqp_table_t client_properties;

				capabilities[0].key = amqp_cstring_bytes("consumer_cancel_notify");
				capabilities[0].value.kind = AMQP_FIELD_KIND_BOOLEAN;
				capabilities[0].value.value.boolean = 1;

				capability_entry.key = amqp_cstring_bytes("capabilities");
				capability_entry.value.kind = AMQP_FIELD_KIND_TABLE;
				capability_entry.value.value.table.num_entries = sizeof(capabilities) / sizeof(amqp_table_entry_t);
				capability_entry.value.value.table.entries = capabilities;

				client_properties.num_entries = 1;
				client_properties.entries = &capability_entry;

				check_rpc_reply(0, amqp_login_with_properties(m_connection, vhost.c_str(), 0, 
					frame_max, heartbeat, &client_properties, AMQP_SASL_METHOD_PLAIN, username.c_str(), password.c_str()));

				broker_version_ = compute_broker_version(m_connection);
			}

			amqp_channel_t get_channel()
			{
				if (cs_open == channels_.at(last_used_channel_))
				{
					channels_[last_used_channel_] = cs_used;
					return last_used_channel_;
				}

				const auto it = std::find(channels_.begin(), channels_.end(), cs_open);
				if (channels_.end() == it)
				{
					const auto new_channel = create_new_channel();
					channels_.at(new_channel) = cs_used;
					return new_channel;
				}

				*it = cs_used;
				return amqp_channel_t(it - channels_.begin());
			}

			void return_channel(const amqp_channel_t channel)
			{
				channels_.at(channel) = cs_open;
				last_used_channel_ = channel;
			}
		
			bool is_channel_open(const amqp_channel_t channel) { return cs_closed != channels_.at(channel); }

			// ReSharper disable once CppMemberFunctionMayBeConst
			bool get_next_frame_from_broker(amqp_frame_t& frame, const std::chrono::microseconds timeout)
			{
				struct timeval* tvp = nullptr;
				struct timeval tv_timeout{};
				memset(&tv_timeout, 0, sizeof(tv_timeout));

				if (timeout != std::chrono::microseconds::max())
				{
					// boost::chrono::seconds.count() returns boost::int_atleast64_t,
					// long can be 32 or 64 bit depending on the platform/arch
					// unless the timeout is something absurd cast to long will be ok, but
					// lets guard against the case where someone does something silly
					assert(std::chrono::duration_cast<std::chrono::seconds>(timeout).count() <
						static_cast<std::chrono::seconds::rep>(std::numeric_limits<long>::max()));

					tv_timeout.tv_sec = static_cast<long>(std::chrono::duration_cast<std::chrono::seconds>(timeout).count());
					tv_timeout.tv_usec = static_cast<long>((timeout - std::chrono::seconds(tv_timeout.tv_sec)).count());
					tvp = &tv_timeout;
				}

				const auto ret = amqp_simple_wait_frame_noblock(m_connection, &frame, tvp);
				if (AMQP_STATUS_TIMEOUT == ret)
				{
					return false;
				}
				check_for_error(ret);
				return true;
			}

			bool check_for_queued_message_on_channel(const amqp_channel_t channel) const
			{
				auto it = std::find_if(frame_queue_.begin(), frame_queue_.end(), 
					[=](amqp_frame_t const& fr){ return is_method_on_channel(fr, AMQP_BASIC_DELIVER_METHOD, channel); });

				if (it == frame_queue_.end())
					return false;

				it = std::find_if(it + 1, frame_queue_.end(), [=](amqp_frame_t const& fr){ return is_on_channel(fr, channel); });
				if (it == frame_queue_.end())
					return false;
				
				if (it->frame_type != AMQP_FRAME_HEADER)
					throw std::runtime_error("Protocol error");

				const auto body_length = it->payload.properties.body_size;
				uint64_t body_received = 0;
				while (body_received < body_length)
				{
					it = std::find_if(it + 1, frame_queue_.end(), [=](amqp_frame_t const& fr){ return is_on_channel(fr, channel); });
					if (it == frame_queue_.end())
						return false;
					if (it->frame_type != AMQP_FRAME_BODY)
						throw std::runtime_error("Protocol error");
					body_received += it->payload.body_fragment.len;
				}

				return true;
			}
			
			void add_to_frame_queue(amqp_frame_t const& frame)
			{
				frame_queue_.push_back(frame);

				if (check_for_queued_message_on_channel(frame.channel))
				{
					const std::array<amqp_channel_t, 1> channel = {{frame.channel}};
					std::shared_ptr<amqp_envelope> envelope;
					if (!consume_message_on_channel_inner(channel, envelope, std::chrono::microseconds::max()))
						throw std::logic_error("ConsumeMessageOnChannelInner returned false unexpectedly");

					delivered_messages_.push_back(envelope);
				}
			}

			template <class ChannelListType>
			bool get_next_frame_from_broker_on_channel(const ChannelListType channels, amqp_frame_t& frame_out, std::chrono::microseconds timeout = std::chrono::microseconds::max())
			{
				auto timeout_left = timeout;
				std::chrono::steady_clock::time_point end_point;
				if (timeout != std::chrono::microseconds::max())
				{
					end_point = std::chrono::steady_clock::now() + timeout;
				}

				amqp_frame_t frame;
				while (get_next_frame_from_broker(frame, timeout_left))
				{
					if (channels.end() != std::find(channels.begin(), channels.end(), frame.channel))
					{
						frame_out = frame;
						return true;
					}

					if (frame.channel == 0)
					{
						// Only thing we care to handle on the channel0 is the connection.close method
						if (AMQP_FRAME_METHOD == frame.frame_type && AMQP_CONNECTION_CLOSE_METHOD == frame.payload.method.id)
						{
							finish_close_connection();
							amqp_exception::throw_with(*reinterpret_cast<amqp_connection_close_t *>(frame.payload.method.decoded));
						}
					}
					else
					{
						add_to_frame_queue(frame);
					}

					if (timeout != std::chrono::microseconds::max())
					{
						auto now = std::chrono::steady_clock::now();
						if (now >= end_point)
						{
							return false;
						}
						timeout_left = std::chrono::duration_cast<std::chrono::microseconds>(end_point - now);
					}
				}
				return false;
			}

			bool get_next_frame_on_channel(const amqp_channel_t channel, amqp_frame_t& frame, const std::chrono::microseconds timeout = std::chrono::microseconds::max())
			{
				const auto it = std::find_if(frame_queue_.begin(), frame_queue_.end(), [=](amqp_frame_t const& fr){ return is_on_channel(fr, channel); });
				if (frame_queue_.end() != it)
				{
					frame = *it;
					frame_queue_.erase(it);

					if (AMQP_FRAME_METHOD == frame.frame_type && AMQP_CHANNEL_CLOSE_METHOD == frame.payload.method.id)
					{
						finish_close_channel(channel);
						amqp_exception::throw_with(*reinterpret_cast<amqp_channel_close_t *>(frame.payload.method.decoded));
					}
					return true;
				}

				const std::array<amqp_channel_t, 1> channels = {{channel}};
				return get_next_frame_from_broker_on_channel(channels, frame, timeout);
			}

			static bool is_on_channel(amqp_frame_t const& frame, const amqp_channel_t channel) { return channel == frame.channel; }
			static bool is_frame_type_on_channel(amqp_frame_t const& frame, const uint8_t frame_type, const amqp_channel_t channel) { return frame.frame_type == frame_type && frame.channel == channel; }
			static bool is_method_on_channel(amqp_frame_t const& frame, const amqp_method_number_t method, const amqp_channel_t channel) { return frame.channel == channel && frame.frame_type == AMQP_FRAME_METHOD && frame.payload.method.id == method; }

			template <class ChannelListType, class ResponseListType>
			static bool is_expected_method_on_channel(amqp_frame_t const& frame, const ChannelListType channels, ResponseListType const& expected_responses)
			{
				return channels.end() != std::find(channels.begin(), channels.end(), frame.channel) && AMQP_FRAME_METHOD == frame.frame_type 
					&& expected_responses.end() != std::find(expected_responses.begin(), expected_responses.end(), frame.payload.method.id);
			}

			template <class ChannelListType, class ResponseListType>
			bool get_method_on_channel(const ChannelListType channels, amqp_frame_t& frame, const ResponseListType& expected_responses, const std::chrono::microseconds timeout = std::chrono::microseconds::max())
			{
				const auto desired_frame = std::find_if(frame_queue_.begin(), frame_queue_.end(),
					[=](amqp_frame_t const& fr){ return is_expected_method_on_channel(fr, channels, expected_responses); });

				if (frame_queue_.end() != desired_frame)
				{
					frame = *desired_frame;
					frame_queue_.erase(desired_frame);
					return true;
				}

				std::chrono::steady_clock::time_point end_point;
				auto timeout_left = timeout;
				if (timeout != std::chrono::microseconds::max())
				{
					end_point = std::chrono::steady_clock::now() + timeout;
				}

				amqp_frame_t incoming_frame;
				while (get_next_frame_from_broker_on_channel(channels, incoming_frame, timeout_left))
				{
					if (is_expected_method_on_channel(incoming_frame, channels, expected_responses))
					{
						frame = incoming_frame;
						return true;
					}
					if (AMQP_FRAME_METHOD == incoming_frame.frame_type && AMQP_CHANNEL_CLOSE_METHOD == incoming_frame.payload.method.id)
					{
						finish_close_channel(incoming_frame.channel);
						try
						{
							amqp_exception::throw_with(*reinterpret_cast<amqp_channel_close_t *>(incoming_frame.payload.method.decoded));
						}
						catch (amqp_exception&)
						{
							maybe_release_buffers_on_channel(incoming_frame.channel);
							throw;
						}
					}
					frame_queue_.push_back(incoming_frame);

					if (timeout != std::chrono::microseconds::max())
					{
						auto now = std::chrono::steady_clock::now();
						if (now >= end_point)
							return false;
						timeout_left = std::chrono::duration_cast<std::chrono::microseconds>(end_point - now);
					}
				}
				return false;
			}

			template <class ResponseListType>
			amqp_frame_t do_rpc_on_channel(const amqp_channel_t channel, const uint32_t method_id, void* decoded, const ResponseListType& expected_responses)
			{
				check_for_error(amqp_send_method(m_connection, channel, method_id, decoded));

				amqp_frame_t response;
				std::array<amqp_channel_t, 1> channels = {{channel}};

				get_method_on_channel(channels, response, expected_responses);
				return response;
			}

			template <class ResponseListType>
			amqp_frame_t do_rpc(uint32_t method_id, void* decoded, ResponseListType const& expected_responses)
			{
				auto channel = get_channel();
				const amqp_frame_t ret = do_rpc_on_channel(channel, method_id, decoded, expected_responses);
				return_channel(channel);
				return ret;
			}

			template <class ChannelListType>
			static bool envelope_on_channel(std::shared_ptr<amqp_envelope> const& envelope, const ChannelListType channels)
			{
				return channels.end() != std::find(channels.begin(), channels.end(), envelope->delivery_channel());
			}

			template <class ChannelListType>
			bool consume_message_on_channel(const ChannelListType channels, std::shared_ptr<amqp_envelope>& message, const std::chrono::microseconds timeout)
			{
				const auto it = std::find_if(delivered_messages_.begin(), delivered_messages_.end(),
					[=](std::shared_ptr<amqp_envelope> const& env) { return envelope_on_channel(env, channels); });

				if (it != delivered_messages_.end())
				{
					message = *it;
					delivered_messages_.erase(it);
					return true;
				}

				return consume_message_on_channel_inner(channels, message, timeout);
			}

			template <class ChannelListType>
			bool consume_message_on_channel_inner(const ChannelListType channels, std::shared_ptr<amqp_envelope>& message, const std::chrono::microseconds timeout)
			{
				static const std::array<uint32_t, 2> DELIVER_OR_CANCEL = { {AMQP_BASIC_DELIVER_METHOD, AMQP_BASIC_CANCEL_METHOD} };

				amqp_frame_t deliver;
				if (!get_method_on_channel(channels, deliver, DELIVER_OR_CANCEL, timeout))
				{
					return false;
				}

				if (AMQP_BASIC_CANCEL_METHOD == deliver.payload.method.id)
				{
					const auto cancel_method = reinterpret_cast<amqp_basic_cancel_t *>(deliver.payload.method.decoded);
					const std::string consumer_tag(static_cast<char *>(cancel_method->consumer_tag.bytes), cancel_method->consumer_tag.len);

					remove_consumer(consumer_tag);
					return_channel(deliver.channel);
					maybe_release_buffers_on_channel(deliver.channel);

					throw consumer_cancelled_exception(consumer_tag);
				}

				const auto deliver_method = reinterpret_cast<amqp_basic_deliver_t *>(deliver.payload.method.decoded);

				const std::string exchange(static_cast<char *>(deliver_method->exchange.bytes), deliver_method->exchange.len);
				const std::string routing_key(static_cast<char *>(deliver_method->routing_key.bytes), deliver_method->routing_key.len);
				const std::string in_consumer_tag(static_cast<char *>(deliver_method->consumer_tag.bytes), deliver_method->consumer_tag.len);
				const auto delivery_tag = deliver_method->delivery_tag;
				const auto redelivered = deliver_method->redelivered != 0;
				maybe_release_buffers_on_channel(deliver.channel);

				auto content = read_content(deliver.channel);
				maybe_release_buffers_on_channel(deliver.channel);

				message = std::make_shared<amqp_envelope>(content, in_consumer_tag, delivery_tag, exchange, redelivered, routing_key, deliver.channel);
				return true;
			}

			amqp_channel_t create_new_channel()
			{
				const auto new_channel = get_next_channel_id();
				static const std::array<uint32_t, 1> OPEN_OK = { {AMQP_CHANNEL_OPEN_OK_METHOD} };
				amqp_channel_open_t channel_open = {};
				do_rpc_on_channel<std::array<uint32_t, 1>>(new_channel, AMQP_CHANNEL_OPEN_METHOD, &channel_open, OPEN_OK);
				
				static const std::array<uint32_t, 1> CONFIRM_OK = { {AMQP_CONFIRM_SELECT_OK_METHOD} };
				amqp_confirm_select_t confirm_select = {};
				do_rpc_on_channel<std::array<uint32_t, 1>>(new_channel, AMQP_CONFIRM_SELECT_METHOD, &confirm_select, CONFIRM_OK);

				channels_.at(new_channel) = cs_open;
				return new_channel;
			}

			amqp_channel_t get_next_channel_id()
			{
				auto unused_channel = std::find(channels_.begin(), channels_.end(), cs_closed);
				if (channels_.end() == unused_channel)
				{
					auto max_channels = amqp_get_channel_max(m_connection);
					if (0 == max_channels)
						max_channels = std::numeric_limits<uint16_t>::max();
					if (static_cast<size_t>(max_channels) < channels_.size())
						throw std::runtime_error("Too many channels open");

					channels_.push_back(cs_closed);
					unused_channel = channels_.end() - 1;
				}

				return amqp_channel_t(unused_channel - channels_.begin());
			}

			void check_rpc_reply(const amqp_channel_t channel, amqp_rpc_reply_t const& reply)
			{
				switch (reply.reply_type)
				{
				case AMQP_RESPONSE_NORMAL:
					return;
				case AMQP_RESPONSE_LIBRARY_EXCEPTION:
					// If we're getting this likely is the socket is already closed
					throw amqp_response_library_exception::create_exception(reply, "");
				case AMQP_RESPONSE_SERVER_EXCEPTION:
					if (reply.reply.id == AMQP_CHANNEL_CLOSE_METHOD)
						finish_close_channel(channel);
					else if (reply.reply.id == AMQP_CONNECTION_CLOSE_METHOD)
						finish_close_connection();
					amqp_exception::throw_with(reply);
					break;
				default:
					amqp_exception::throw_with(reply);
				}
			}

			void check_for_error(const int ret) const
			{
				if (ret < 0) throw amqp_library_exception::create_exception(ret);
			}

			void check_frame_for_close(amqp_frame_t& frame, const amqp_channel_t channel)
			{
				if (frame.frame_type == AMQP_FRAME_METHOD)
				{
					switch (frame.payload.method.id)
					{
					case AMQP_CHANNEL_CLOSE_METHOD:
						finish_close_channel(channel);
						amqp_exception::throw_with(*reinterpret_cast<amqp_channel_close_t *>(frame.payload.method.decoded));
						break;
					case AMQP_CONNECTION_CLOSE_METHOD:
						finish_close_connection();
						amqp_exception::throw_with(*reinterpret_cast<amqp_connection_close_t *>(frame.payload.method.decoded));
						break;
					default: break;
					}
				}
			}
			
			void finish_close_channel(const amqp_channel_t channel)
			{
				channels_.at(channel) = cs_closed;
				amqp_channel_close_ok_t close_ok;
				check_for_error(amqp_send_method(m_connection, channel, AMQP_CHANNEL_CLOSE_OK_METHOD, &close_ok));
			}
			
			void finish_close_connection()
			{
				set_is_connected(false);
				amqp_connection_close_ok_t close_ok;
				amqp_send_method(m_connection, 0, AMQP_CONNECTION_CLOSE_OK_METHOD, &close_ok);
			}

			std::shared_ptr<amqp_message> read_content(const amqp_channel_t channel)
			{
				amqp_frame_t frame;
				get_next_frame_on_channel(channel, frame);

				if (frame.frame_type != AMQP_FRAME_HEADER)
					// TODO: We should connection.close here
					throw std::runtime_error("amqp_channel::consume_message: received unexpected frame type (was expected AMQP_FRAME_HEADER)");

				// The memory for this is allocated in a pool associated with the connection
				// The BasicMessage constructor does a deep copy of the properties structure
				auto properties = reinterpret_cast<amqp_basic_properties_t *>(frame.payload.properties.decoded);

				// size_t could possibly be 32-bit, body_size is always 64-bit
				assert(frame.payload.properties.body_size < std::numeric_limits<size_t>::max());

				const auto body_size = static_cast<size_t>(frame.payload.properties.body_size);
				size_t received_size = 0;
				auto body = amqp_bytes_malloc(body_size);

				// frame #3 and up:
				while (received_size < body_size)
				{
					get_next_frame_on_channel(channel, frame);

					if (frame.frame_type != AMQP_FRAME_BODY)
						// TODO: we should connection.close here
						throw std::runtime_error("amqp_channel::BasicConsumeMessage: received unexpected frame type (was expecting AMQP_FRAME_BODY)");

					void* body_ptr = reinterpret_cast<char *>(body.bytes) + received_size;
					memcpy(body_ptr, frame.payload.body_fragment.bytes, frame.payload.body_fragment.len);
					received_size += frame.payload.body_fragment.len;
				}
				return std::make_shared<amqp_message>(body, properties);
			}

			void add_consumer(std::string const& consumer_tag, amqp_channel_t channel)
			{
				consumer_channel_map_.insert(std::make_pair(consumer_tag, channel));
			}

			amqp_channel_t remove_consumer(std::string const& consumer_tag)
			{
				const auto it = consumer_channel_map_.find(consumer_tag);
				if (it == consumer_channel_map_.end())
					throw consumer_tag_not_found_exception();

				const auto result = it->second;
				consumer_channel_map_.erase(it);
				return result;
			}

			amqp_channel_t get_consumer_channel(std::string const& consumer_tag)
			{
				const auto it = consumer_channel_map_.find(consumer_tag);
				if (it == consumer_channel_map_.end())
					throw consumer_tag_not_found_exception();
				return it->second;
			}

			std::vector<amqp_channel_t> get_all_consumer_channels() const
			{
				std::vector<amqp_channel_t> ret;
				for (auto const& it : consumer_channel_map_)
					ret.push_back(it.second);
				return ret;
			}

			void maybe_release_buffers_on_channel(amqp_channel_t channel)
			{
				if (frame_queue_.end() == std::find_if(frame_queue_.begin(), frame_queue_.end(),
					[=](const amqp_frame_t ch) { return is_on_channel(ch, channel); }))
				{
					amqp_maybe_release_buffers_on_channel(m_connection, channel);
				}
			}

			void check_is_connected() const
			{
				if (!is_connected_) throw connection_closed_exception();
			}

			void set_is_connected(const bool state) { is_connected_ = state; }

			// The RabbitMQ broker changed the way that basic.qos worked as of v3.3.0.
			// See: http://www.rabbitmq.com/consumer-prefetch.html
			// Newer versions of RabbitMQ basic.qos.global set to false applies to new
			// consumers made on the channel, and true applies to all consumers on the
			// channel (not connection).
			bool broker_has_new_qos_behavior() const { return 0x030300 <= broker_version_; }

			amqp_connection_state_t m_connection{};

		private:
			static uint32_t compute_broker_version(const amqp_connection_state_t state)  // NOLINT(misc-misplaced-const)
			{
				const amqp_table_t* properties = amqp_get_server_properties(state);
				const auto version = amqp_cstring_bytes("version");
				amqp_table_entry_t* version_entry = nullptr;

				for (auto i = 0; i < properties->num_entries; ++i)
				{
					if (properties->entries[i].key == version)
					{
						version_entry = &properties->entries[i];
						break;
					}
				}
				if (nullptr == version_entry)
					return 0;

				std::string version_string(static_cast<char *>(version_entry->value.value.bytes.bytes), version_entry->value.value.bytes.len);
				std::vector<std::string> version_components;
				boost::split(version_components, version_string, boost::is_any_of("."));
				if (version_components.size() != 3)
					return 0;

				const auto version_major = boost::lexical_cast<uint32_t>(version_components[0]);
				const auto version_minor = boost::lexical_cast<uint32_t>(version_components[1]);
				const auto version_patch = boost::lexical_cast<uint32_t>(version_components[2]);
				return (version_major & 0xFF) << 16 | (version_minor & 0xFF) << 8 | (version_patch & 0xFF);
			}

			bool is_connected_;
			frame_queue_t frame_queue_;
			std::vector<std::shared_ptr<amqp_envelope>> delivered_messages_;
			std::map<std::string, amqp_channel_t> consumer_channel_map_;
			amqp_channel_t last_used_channel_;  // A channel that is likely to be an CS_Open state
			std::vector<channel_states> channels_;
			uint32_t broker_version_{};
		};


		amqp_channel::amqp_channel(amqp_host const& host, int heartbeat, int frame_max)
			: impl_(new amqp_channel_impl)
		{
			impl_->m_connection = amqp_new_connection();

			if (nullptr == impl_->m_connection)
				throw std::bad_alloc();

			try
			{
				amqp_socket_t_ * socket;
				if (auto ssl = host.ssl)
				{
					socket = amqp_ssl_socket_new(impl_->m_connection);
					if (nullptr == socket)
						throw std::bad_alloc();

#if AMQP_VERSION >= 0x00080001
					amqp_ssl_socket_set_verify_peer(socket, ssl->verify_hostname);
					amqp_ssl_socket_set_verify_hostname(socket, ssl->verify_hostname);
#else
					amqp_ssl_socket_set_verify(socket, ssl->verify_hostname);
#endif

					auto status = amqp_ssl_socket_set_cacert(socket, ssl->path_to_ca_cert.c_str());
					if (status)
						throw amqp_library_exception::create_exception(status, "Error setting CA certificate for socket");

					if (!ssl->path_to_client_key.empty() && !ssl->path_to_client_cert.empty())
					{
						status = amqp_ssl_socket_set_key(socket, ssl->path_to_client_cert.c_str(), ssl->path_to_client_key.c_str());
						if (status)
							throw amqp_library_exception::create_exception(status, "Error setting client certificate for socket");
					}
				}
				else
				{
					socket = amqp_tcp_socket_new(impl_->m_connection);
				}
				
				const auto res = amqp_socket_open(socket, host.host.c_str(), host.port);
				impl_->check_for_error(res);
				impl_->do_login(host.user, host.password, host.vhost, heartbeat, frame_max);
			}
			catch (...)
			{
				amqp_destroy_connection(impl_->m_connection);
				throw;
			}

			impl_->set_is_connected(true);
		}
		
		amqp_channel::~amqp_channel()
		{
			amqp_connection_close(impl_->m_connection, AMQP_REPLY_SUCCESS);
			amqp_destroy_connection(impl_->m_connection);
		}

		void amqp_channel::declare_exchange(std::string const& exchange_name, std::string const& exchange_type,
		                                   const bool passive, const bool durable, const bool auto_delete, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> DECLARE_OK = { {AMQP_EXCHANGE_DECLARE_OK_METHOD} };
			impl_->check_is_connected();

			amqp_exchange_declare_t declare = {};
			declare.exchange = amqp_cstring_bytes(exchange_name.c_str());
			declare.type = amqp_cstring_bytes(exchange_type.c_str());
			declare.passive = passive;
			declare.durable = durable;
			declare.auto_delete = auto_delete;
			declare.internal = false;
			declare.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			declare.arguments = args.to_amqp(table_pool);

			const auto frame = impl_->do_rpc(AMQP_EXCHANGE_DECLARE_METHOD, &declare, DECLARE_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::delete_exchange(std::string const& exchange_name, const bool if_unused) const
		{
			static const std::array<uint32_t, 1> DELETE_OK = {{AMQP_EXCHANGE_DELETE_OK_METHOD}};
			impl_->check_is_connected();

			amqp_exchange_delete_t del = {};
			del.exchange = amqp_cstring_bytes(exchange_name.c_str());
			del.if_unused = if_unused;
			del.nowait = false;

			const auto frame = impl_->do_rpc(AMQP_EXCHANGE_DELETE_METHOD, &del, DELETE_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}
		
		void amqp_channel::bind_exchange(std::string const& destination, std::string const& source, std::string const& routing_key, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> BIND_OK = {{AMQP_EXCHANGE_BIND_OK_METHOD}};
			impl_->check_is_connected();

			amqp_exchange_bind_t bind = {};
			bind.destination = amqp_cstring_bytes(destination.c_str());
			bind.source = amqp_cstring_bytes(source.c_str());
			bind.routing_key = amqp_cstring_bytes(routing_key.c_str());
			bind.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			bind.arguments = args.to_amqp(table_pool);

			const auto frame = impl_->do_rpc(AMQP_EXCHANGE_BIND_METHOD, &bind, BIND_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::unbind_exchange(std::string const& destination, std::string const& source, std::string const& routing_key, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> UNBIND_OK = {{AMQP_EXCHANGE_UNBIND_OK_METHOD}};
			impl_->check_is_connected();

			amqp_exchange_unbind_t unbind = {};
			unbind.destination = amqp_cstring_bytes(destination.c_str());
			unbind.source = amqp_cstring_bytes(source.c_str());
			unbind.routing_key = amqp_cstring_bytes(routing_key.c_str());
			unbind.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			unbind.arguments = args.to_amqp(table_pool);

			const auto frame = impl_->do_rpc(AMQP_EXCHANGE_UNBIND_METHOD, &unbind, UNBIND_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		std::string amqp_channel::declare_queue(std::string const& queue_name, const bool passive,
		                                       const bool durable, const bool exclusive, const bool auto_delete, amqp_table const& args) const
		{
			uint32_t message_count;
			uint32_t consumer_count;
			return declare_queue_with_counts(queue_name, message_count, consumer_count, passive, durable, exclusive, auto_delete, args);
		}

		std::string amqp_channel::declare_queue_with_counts(std::string const& queue_name, uint32_t& message_count, uint32_t& consumer_count,
		                                                 const bool passive, const bool durable, const bool exclusive, const bool auto_delete, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> DECLARE_OK = {{AMQP_QUEUE_DECLARE_OK_METHOD}};
			impl_->check_is_connected();

			amqp_queue_declare_t declare = {};
			declare.queue = amqp_cstring_bytes(queue_name.c_str());
			declare.passive = passive;
			declare.durable = durable;
			declare.exclusive = exclusive;
			declare.auto_delete = auto_delete;
			declare.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			declare.arguments = args.to_amqp(table_pool);

			const auto response = impl_->do_rpc(AMQP_QUEUE_DECLARE_METHOD, &declare, DECLARE_OK);
			const auto declare_ok = static_cast<amqp_queue_declare_ok_t *>(response.payload.method.decoded);

			std::string ret(static_cast<char *>(declare_ok->queue.bytes), declare_ok->queue.len);
			message_count = declare_ok->message_count;
			consumer_count = declare_ok->consumer_count;

			impl_->maybe_release_buffers_on_channel(response.channel);
			return ret;
		}

		void amqp_channel::delete_queue(std::string const& queue_name, const bool if_unused, const bool if_empty) const
		{
			static const std::array<uint32_t, 1> DELETE_OK = {{AMQP_QUEUE_DELETE_OK_METHOD}};
			impl_->check_is_connected();

			amqp_queue_delete_t del = {};
			del.queue = amqp_cstring_bytes(queue_name.c_str());
			del.if_unused = if_unused;
			del.if_empty = if_empty;
			del.nowait = false;

			const auto frame = impl_->do_rpc(AMQP_QUEUE_DELETE_METHOD, &del, DELETE_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::bind_queue(std::string const& queue_name, std::string const& exchange_name, std::string const& routing_key, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> BIND_OK = {{AMQP_QUEUE_BIND_OK_METHOD}};
			impl_->check_is_connected();

			amqp_queue_bind_t bind = {};
			bind.queue = amqp_cstring_bytes(queue_name.c_str());
			bind.exchange = amqp_cstring_bytes(exchange_name.c_str());
			bind.routing_key = amqp_cstring_bytes(routing_key.c_str());
			bind.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			bind.arguments = args.to_amqp(table_pool);

			const auto frame = impl_->do_rpc(AMQP_QUEUE_BIND_METHOD, &bind, BIND_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::unbind_queue(std::string const& queue_name, std::string const& exchange_name, std::string const& routing_key, amqp_table const& args) const
		{
			static const std::array<uint32_t, 1> UNBIND_OK = {{AMQP_QUEUE_UNBIND_OK_METHOD}};
			impl_->check_is_connected();

			amqp_queue_unbind_t unbind = {};
			unbind.queue = amqp_cstring_bytes(queue_name.c_str());
			unbind.exchange = amqp_cstring_bytes(exchange_name.c_str());
			unbind.routing_key = amqp_cstring_bytes(routing_key.c_str());

			std::shared_ptr<amqp_pool_t> table_pool;
			unbind.arguments = args.to_amqp(table_pool);

			const auto frame = impl_->do_rpc(AMQP_QUEUE_UNBIND_METHOD, &unbind, UNBIND_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::purge_queue(std::string const& queue_name) const
		{
			static const std::array<uint32_t, 1> PURGE_OK = {{AMQP_QUEUE_PURGE_OK_METHOD}};
			impl_->check_is_connected();

			amqp_queue_purge_t purge = {};
			purge.queue = amqp_cstring_bytes(queue_name.c_str());
			purge.nowait = false;

			const auto frame = impl_->do_rpc(AMQP_QUEUE_PURGE_METHOD, &purge, PURGE_OK);
			impl_->maybe_release_buffers_on_channel(frame.channel);
		}

		void amqp_channel::ack(amqp_envelope::delivery_info const& info) const
		{
			impl_->check_is_connected();
			// Delivery tag is local to the channel, so its important to use
			// that channel, sadly this can cause the channel to throw an exception
			// which will show up as an unrelated exception in a different method
			// that actually waits for a response from the broker
			const auto channel = info.delivery_channel;
			if (!impl_->is_channel_open(channel))
				throw std::runtime_error("The channel that the message was delivered on has been closed");

			impl_->check_for_error(amqp_basic_ack(impl_->m_connection, channel, info.delivery_tag, false));
		}

		void amqp_channel::reject(amqp_envelope::delivery_info const& info, const bool requeue, const bool multiple) const
		{
			impl_->check_is_connected();
			// Delivery tag is local to the channel, so its important to use
			// that channel, sadly this can cause the channel to throw an exception
			// which will show up as an unrelated exception in a different method
			// that actually waits for a response from the broker
			const auto channel = info.delivery_channel;
			if (!impl_->is_channel_open(channel))
				throw std::runtime_error("The channel that the message was delivered on has been closed");
			
			amqp_basic_nack_t req;
			req.delivery_tag = info.delivery_tag;
			req.multiple = multiple;
			req.requeue = requeue;

			impl_->check_for_error(amqp_send_method(impl_->m_connection, channel, AMQP_BASIC_NACK_METHOD, &req));
		}

		void amqp_channel::publish(std::string const& exchange_name, std::string const& routing_key,
		                           std::shared_ptr<amqp_message> const& message, const bool mandatory, const bool immediate) const
		{
			impl_->check_is_connected();
			const auto channel = impl_->get_channel();

			impl_->check_for_error(amqp_basic_publish(impl_->m_connection, channel, amqp_cstring_bytes(exchange_name.c_str()),
				amqp_cstring_bytes(routing_key.c_str()), mandatory, immediate, message->get_amqp_properties(), message->get_amqp_body()));

			// If we've done things correctly we can get one of 4 things back from the broker
			// - basic.ack - our channel is in confirm mode, message was 'dealt with' by the broker
			// - basic.return then basic.ack - the message wasn't delivered, but was dealt with
			// - channel.close - probably tried to publish to a non-existent exchange, in any case error!
			// - connection.close - something really bad happened
			static const std::array<uint32_t, 2> PUBLISH_ACK = {{AMQP_BASIC_ACK_METHOD, AMQP_BASIC_RETURN_METHOD}};

			amqp_frame_t response;
			const std::array<amqp_channel_t, 1> channels = {{channel}};
			impl_->get_method_on_channel(channels, response, PUBLISH_ACK);

			if (AMQP_BASIC_RETURN_METHOD == response.payload.method.id)
			{
				const auto return_method = *(reinterpret_cast<amqp_basic_return_t *>(response.payload.method.decoded));
				const int reply_code = return_method.reply_code;
				const std::string reply_text(static_cast<char *>(return_method.reply_text.bytes), return_method.reply_text.len);
				const std::string exchange(static_cast<char *>(return_method.exchange.bytes), return_method.exchange.len);
				const std::string route_key(static_cast<char *>(return_method.routing_key.bytes), return_method.routing_key.len);
				const auto content = impl_->read_content(channel);
				
				static const std::array<uint32_t, 1> BASIC_ACK = {{AMQP_BASIC_ACK_METHOD}};
				impl_->get_method_on_channel(channels, response, BASIC_ACK);
				impl_->return_channel(channel);
				impl_->maybe_release_buffers_on_channel(channel);
				throw message_returned_exception(content, reply_code, reply_text, exchange, route_key);
			}

			impl_->return_channel(channel);
			impl_->maybe_release_buffers_on_channel(channel);
		}

		bool amqp_channel::get(std::shared_ptr<amqp_envelope>& envelope, std::string const& queue, const bool no_ack) const
		{
			static const std::array<uint32_t, 2> GET_RESPONSES = {{AMQP_BASIC_GET_OK_METHOD, AMQP_BASIC_GET_EMPTY_METHOD}};
			impl_->check_is_connected();

			amqp_basic_get_t get = {};
			get.queue = amqp_cstring_bytes(queue.c_str());
			get.no_ack = no_ack;

			auto channel = impl_->get_channel();
			const auto response = impl_->do_rpc_on_channel(channel, AMQP_BASIC_GET_METHOD, &get, GET_RESPONSES);

			if (AMQP_BASIC_GET_EMPTY_METHOD == response.payload.method.id)
			{
				impl_->return_channel(channel);
				impl_->maybe_release_buffers_on_channel(channel);
				return false;
			}

			const auto get_ok = static_cast<amqp_basic_get_ok_t *>(response.payload.method.decoded);
			auto delivery_tag = get_ok->delivery_tag;
			auto redelivered = get_ok->redelivered != 0;
			std::string exchange(static_cast<char *>(get_ok->exchange.bytes), get_ok->exchange.len);
			std::string routing_key(static_cast<char *>(get_ok->routing_key.bytes), get_ok->routing_key.len);

			auto message = impl_->read_content(channel);
			envelope = std::make_shared<amqp_envelope>(message, "", delivery_tag, exchange, redelivered, routing_key, channel);

			impl_->return_channel(channel);
			impl_->maybe_release_buffers_on_channel(channel);
			return true;
		}

		void amqp_channel::recover(std::string const& consumer) const
		{
			static const std::array<uint32_t, 1> RECOVER_OK = {{AMQP_BASIC_RECOVER_OK_METHOD}};
			impl_->check_is_connected();

			amqp_basic_recover_t recover;
			recover.requeue = true;

			const auto channel = impl_->get_consumer_channel(consumer);
			impl_->do_rpc_on_channel(channel, AMQP_BASIC_RECOVER_METHOD, &recover, RECOVER_OK);
			impl_->maybe_release_buffers_on_channel(channel);
		}

		std::string amqp_channel::consume(std::string const& queue, std::string const& consumer_tag, const bool no_local,
		                                       const bool no_ack, const bool exclusive, const uint16_t prefetch_count, amqp_table const& args) const
		{
			impl_->check_is_connected();
			const auto channel = impl_->get_channel();

			// Set this before starting the consume as it may have been set by a previous consumer
			static const std::array<uint32_t, 1> QOS_OK = {{AMQP_BASIC_QOS_OK_METHOD}};

			amqp_basic_qos_t qos;
			qos.prefetch_size = 0;
			qos.prefetch_count = prefetch_count;
			qos.global = impl_->broker_has_new_qos_behavior();

			impl_->do_rpc_on_channel(channel, AMQP_BASIC_QOS_METHOD, &qos, QOS_OK);
			impl_->maybe_release_buffers_on_channel(channel);

			static const std::array<uint32_t, 1> CONSUME_OK = {{AMQP_BASIC_CONSUME_OK_METHOD}};

			amqp_basic_consume_t consume = {};
			consume.queue = amqp_cstring_bytes(queue.c_str());
			consume.consumer_tag = amqp_cstring_bytes(consumer_tag.c_str());
			consume.no_local = no_local;
			consume.no_ack = no_ack;
			consume.exclusive = exclusive;
			consume.nowait = false;

			std::shared_ptr<amqp_pool_t> table_pool;
			consume.arguments = args.to_amqp(table_pool);

			const auto response = impl_->do_rpc_on_channel(channel, AMQP_BASIC_CONSUME_METHOD, &consume, CONSUME_OK);
			const auto consume_ok = static_cast<amqp_basic_consume_ok_t *>(response.payload.method.decoded);
			std::string tag(static_cast<char *>(consume_ok->consumer_tag.bytes), consume_ok->consumer_tag.len);
			impl_->maybe_release_buffers_on_channel(channel);
			impl_->add_consumer(tag, channel);
			return tag;
		}

		void amqp_channel::qos(std::string const& consumer_tag, const uint16_t prefetch_count) const
		{
			impl_->check_is_connected();
			const auto channel = impl_->get_consumer_channel(consumer_tag);
			
			static const std::array<uint32_t, 1> QOS_OK = {{AMQP_BASIC_QOS_OK_METHOD}};

			amqp_basic_qos_t qos;
			qos.prefetch_size = 0;
			qos.prefetch_count = prefetch_count;
			qos.global = impl_->broker_has_new_qos_behavior();

			impl_->do_rpc_on_channel(channel, AMQP_BASIC_QOS_METHOD, &qos, QOS_OK);
			impl_->maybe_release_buffers_on_channel(channel);
		}

		void amqp_channel::cancel(std::string const& consumer_tag) const
		{
			impl_->check_is_connected();
			const auto channel = impl_->get_consumer_channel(consumer_tag);

			static const std::array<uint32_t, 1> CANCEL_OK = {{AMQP_BASIC_CANCEL_OK_METHOD}};

			amqp_basic_cancel_t cancel;
			cancel.consumer_tag = amqp_cstring_bytes(consumer_tag.c_str());
			cancel.nowait = false;

			impl_->do_rpc_on_channel(channel, AMQP_BASIC_CANCEL_METHOD, &cancel, CANCEL_OK);
			impl_->remove_consumer(consumer_tag);

			// Lets go hunting to make sure we don't have any queued frames lying around
			// Otherwise these frames will potentially hang around when we don't want them to
			// TODO: Implement queue purge
			impl_->return_channel(channel);
			impl_->maybe_release_buffers_on_channel(channel);
		}

		bool amqp_channel::consume_message(std::string const& consumer_tag, std::shared_ptr<amqp_envelope>& envelope, const std::chrono::microseconds timeout) const
		{
			impl_->check_is_connected();
			const auto channel = impl_->get_consumer_channel(consumer_tag);
			const std::array<amqp_channel_t, 1> channels = {{channel}};
			return impl_->consume_message_on_channel(channels, envelope, timeout);
		}

		bool amqp_channel::consume_message(std::vector<std::string> const& consumer_tags, std::shared_ptr<amqp_envelope>& envelope, const std::chrono::microseconds timeout) const
		{
			impl_->check_is_connected();

			std::vector<amqp_channel_t> channels;
			channels.reserve(consumer_tags.size());
			for (const auto& consumer_tag : consumer_tags)
				channels.push_back(impl_->get_consumer_channel(consumer_tag));

			return impl_->consume_message_on_channel(channels, envelope, timeout);
		}

		bool amqp_channel::consume_message(std::shared_ptr<amqp_envelope>& envelope, const std::chrono::microseconds timeout) const
		{
			impl_->check_is_connected();

			auto channels = impl_->get_all_consumer_channels();
			if (channels.empty())
				throw consumer_tag_not_found_exception();

			return impl_->consume_message_on_channel(channels, envelope, timeout);
		}
	}
}
