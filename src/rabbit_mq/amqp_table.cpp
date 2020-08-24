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

#include "masstransit_cpp/rabbit_mq/amqp_table.hpp"

#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

#include <algorithm>

namespace masstransit_cpp
{
	namespace rabbit_mq
	{
		class generate_field_value : public boost::static_visitor<amqp_field_value_t>  // NOLINT(cppcoreguidelines-special-member-functions, hicpp-special-member-functions)
		{
		public:
			explicit generate_field_value(amqp_pool_t& p) 
				: pool_(p)
			{
			}

			virtual ~generate_field_value() = default;

			amqp_field_value_t operator()(void_t) const;
			amqp_field_value_t operator()(bool value) const;
			amqp_field_value_t operator()(int8_t value) const;
			amqp_field_value_t operator()(int16_t value) const;
			amqp_field_value_t operator()(int32_t value) const;
			amqp_field_value_t operator()(int64_t value) const;
			amqp_field_value_t operator()(float value) const;
			amqp_field_value_t operator()(double value) const;
			amqp_field_value_t operator()(std::string const& value) const;
			amqp_field_value_t operator()(amqp_table const& value) const;

		private:
			amqp_pool_t& pool_;
		};

		amqp_field_value_t generate_field_value::operator()(const void_t) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_VOID;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const bool value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_BOOLEAN;
			v.value.boolean = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const int8_t value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_I8;
			v.value.i8 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const int16_t value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_I16;
			v.value.i16 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const int32_t value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_I32;
			v.value.i32 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const int64_t value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_I64;
			v.value.i64 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const float value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_F32;
			v.value.f32 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(const double value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_F64;
			v.value.f64 = value;
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(std::string const& value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_UTF8;
			amqp_pool_alloc_bytes(&pool_, value.size(), &v.value.bytes);
			memcpy(v.value.bytes.bytes, value.data(), v.value.bytes.len);
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		amqp_field_value_t generate_field_value::operator()(amqp_table const& value) const
		{
			amqp_field_value_t v;
			v.kind = AMQP_FIELD_KIND_TABLE;
			v.value.table = amqp_table(value).to_amqp(pool_);
			// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
			return v;
		}

		void free_pool(amqp_pool_t* pool)
		{
			empty_amqp_pool(pool);
			delete pool;
		}

		amqp_table_t amqp_table::to_amqp(std::shared_ptr<amqp_pool_t> & pool) const
		{
			if (empty())
				return AMQP_EMPTY_TABLE;

			pool = std::shared_ptr<amqp_pool_t>(new amqp_pool_t, free_pool);
			init_amqp_pool(pool.get(), 1024);
			return to_amqp(*pool.get());
		}

		amqp_table_t amqp_table::to_amqp(amqp_pool_t& pool) const
		{
			amqp_table_t new_table;

			new_table.num_entries = int(size());
			new_table.entries = static_cast<amqp_table_entry_t *>(amqp_pool_alloc(&pool, sizeof(amqp_table_entry_t) * size()));

			if (nullptr == new_table.entries)
				throw std::bad_alloc();

			auto output_it = new_table.entries;
			for (auto it = begin(); it != end(); ++it, ++output_it)
			{
				amqp_pool_alloc_bytes(&pool, it->first.size(), &output_it->key);
				if (nullptr == output_it->key.bytes)
					throw std::bad_alloc();

				std::copy(it->first.begin(), it->first.end(), static_cast<char *>(output_it->key.bytes));

				output_it->value = boost::apply_visitor(generate_field_value(pool), it->second);
			}

			return new_table;
		}

		amqp_table::amqp_table() = default;

		amqp_table::amqp_table(amqp_table_t const& table)
		{
			for (auto i = 0; i < table.num_entries; ++i)
			{
				const auto entry = &table.entries[i];
				std::string key(static_cast<char *>(entry->key.bytes), entry->key.len);
				emplace(key, create_value(entry->value));
			}
		}

		amqp_table_value amqp_table::create_value(amqp_field_value_t const& entry)
		{
			switch (entry.kind)
			{
			case AMQP_FIELD_KIND_VOID:
				return amqp_table_value();
			case AMQP_FIELD_KIND_BOOLEAN:
				return amqp_table_value(bool(entry.value.boolean));
			case AMQP_FIELD_KIND_I8:
				return amqp_table_value(entry.value.i8);
			case AMQP_FIELD_KIND_I16:
				return amqp_table_value(entry.value.i16);
			case AMQP_FIELD_KIND_I32:
				return amqp_table_value(entry.value.i32);
			case AMQP_FIELD_KIND_I64:
			case AMQP_FIELD_KIND_TIMESTAMP:
				return amqp_table_value(entry.value.i64);
			case AMQP_FIELD_KIND_F32:
				return amqp_table_value(entry.value.f32);
			case AMQP_FIELD_KIND_F64:
				return amqp_table_value(entry.value.f64);
			case AMQP_FIELD_KIND_UTF8:
			case AMQP_FIELD_KIND_BYTES:
				return amqp_table_value(std::string(static_cast<char *>(entry.value.bytes.bytes), entry.value.bytes.len));
			case AMQP_FIELD_KIND_TABLE:
				return amqp_table_value(amqp_table(entry.value.table));
			case AMQP_FIELD_KIND_DECIMAL:
			default:
				return amqp_table_value();
			}
		}

		amqp_table_t amqp_table::copy_amqp(amqp_table_t const& table, std::shared_ptr<amqp_pool_t>& pool)
		{
			if (0 == table.num_entries)
				return AMQP_EMPTY_TABLE;

			pool = std::shared_ptr<amqp_pool_t>(new amqp_pool_t, free_pool);
			init_amqp_pool(pool.get(), 1024);

			return copy_amqp(table, *pool.get());
		}

		amqp_table_t amqp_table::copy_amqp(amqp_table_t const& table, amqp_pool_t& pool)
		{
			amqp_table_t new_table;

			new_table.num_entries = table.num_entries;
			new_table.entries = static_cast<amqp_table_entry_t *>(amqp_pool_alloc(&pool, sizeof(amqp_table_entry_t) * table.num_entries));
			if (nullptr == new_table.entries)
				throw std::bad_alloc();

			for (auto i = 0; i < table.num_entries; ++i)
			{
				auto entry = &new_table.entries[i];
				amqp_pool_alloc_bytes(&pool, table.entries[i].key.len, &entry->key);
				if (nullptr == entry->key.bytes)
					throw std::bad_alloc();
				
				memcpy(entry->key.bytes, table.entries[i].key.bytes, entry->key.len);
				entry->value = copy_value(table.entries[i].value, pool);
			}

			return new_table;
		}

		amqp_field_value_t amqp_table::copy_value(amqp_field_value_t const& value, amqp_pool_t& pool)
		{
			auto new_value = value;
			switch (value.kind)
			{
			case AMQP_FIELD_KIND_UTF8:
			case AMQP_FIELD_KIND_BYTES:
				amqp_pool_alloc_bytes(&pool, value.value.bytes.len, &new_value.value.bytes);
				memcpy(new_value.value.bytes.bytes, value.value.bytes.bytes, value.value.bytes.len);
				return new_value;
			case AMQP_FIELD_KIND_ARRAY:
				new_value.value.array.entries = static_cast<amqp_field_value_t *>(amqp_pool_alloc(&pool, sizeof(amqp_field_value_t) * value.value.array.num_entries));
				for (auto i = 0; i < value.value.array.num_entries; ++i)
					new_value.value.array.entries[i] = copy_value(value.value.array.entries[i], pool);
				
				// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
				return new_value;
			case AMQP_FIELD_KIND_TABLE:
				new_value.value.table = copy_amqp(value.value.table, pool);
				// ReSharper disable once CppSomeObjectMembersMightNotBeInitialized
				return new_value;
			default:
				return new_value;
			}
		}
	}
}
