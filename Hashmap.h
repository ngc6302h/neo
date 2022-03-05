/*
* Copyright (C) 2021  Iori Torres (shortanemoia@protonmail.com)
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.

*  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.

*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include "Types.h"
#include "Optional.h"
#include "Vector.h"
#include "Bitset.h"
#include "Buffer.h"
#include "Tuple.h"

namespace neo
{
    template<typename T>
    struct DefaultHasher
    {
        static constexpr size_t hash(const T& value)
        {
            u8* data = reinterpret_cast<u8*>(&value);
            size_t size = sizeof(T);
            size_t result = data[size - 1];
            for (size_t i = 0; i < size; ++i)
                result += result ^ (data[i] & ((result * 3241) >> 3));
            return result;
        }
    };

    template<Integral T>
    struct DefaultHasher<T>
    {
        static constexpr size_t hash(T value)
        {
            return value;
        }
    };

    template<typename TKey, typename TValue, typename Hasher = DefaultHasher<TKey>>
    class Hashmap;

    template<typename TKey, typename TValue>
    class HashmapRecord
    {
        template<typename, typename, typename>
        friend class Hashmap;
        template<typename, typename, typename>
        friend class HashmapIteratorContainer;

    public:
        constexpr HashmapRecord(TKey const& key, TValue const& value, HashmapRecord* next) :
            m_key(key), m_value(value), m_next(next)
        {
        }

        constexpr HashmapRecord(TKey const& key, TValue&& value, HashmapRecord* next) :
            m_key(key), m_value(forward<TValue>(value)), m_next(next)
        {
        }

        constexpr TValue& value()
        {
            return m_value;
        }

        constexpr TValue const& value() const
        {
            return m_value;
        }

        constexpr TKey const& key() const
        {
            return m_key;
        }

    private:
        TKey m_key;
        TValue m_value;
        HashmapRecord* m_next;
    };

    template<typename THashmap, typename TKey, typename TValue>
    class HashmapIteratorContainer
    {
    public:
        constexpr HashmapIteratorContainer(THashmap& hashmap, bool end) :
            m_hashmap(hashmap),
            m_bucket(end ? hashmap.m_buckets.size() : 0),
            m_element_offset(end ? hashmap.m_buckets[0].size() : 0),
            m_next(nullptr)
        {
        }

        constexpr HashmapIteratorContainer(HashmapIteratorContainer const& other) :
            m_hashmap(other.m_hashmap),
            m_bucket(other.m_bucket),
            m_element_offset(other.m_element_offset),
            m_next(other.m_next)
        {
        }

        constexpr HashmapIteratorContainer(HashmapIteratorContainer&& other) :
            m_hashmap(other.m_hashmap),
            m_bucket(other.m_bucket),
            m_element_offset(other.m_element_offset),
            m_next(other.m_next)
        {
            other.m_next = nullptr;
            other.m_element_offset = 0;
            other.m_bucket = 0;
        }

        constexpr HashmapIteratorContainer& operator=(HashmapIteratorContainer const& other)
        {
            if (this == &other)
                return *this;

            new (this) HashmapIteratorContainer { other };
            return *this;
        }

        constexpr HashmapIteratorContainer& operator=(HashmapIteratorContainer&& other)
        {
            if (this == &other)
                return *this;

            new (this) HashmapIteratorContainer { move(other) };
            return *this;
        }

        constexpr HashmapIteratorContainer operator++()
        {
            auto prev = *this;
            ++*this;
            return prev;
        }

        constexpr HashmapIteratorContainer& operator++(int)
        {
            if (m_next != nullptr)
            {
                m_next = m_next->m_next;
                return *this;
            }

            if (m_element_offset < m_hashmap.m_buckets[m_bucket].size())
            {
                do
                    m_element_offset++;
                while (m_hashmap.m_buckets[m_bucket][m_element_offset].m_next == nullptr);
                return *this;
            }

            if (m_bucket < m_hashmap.m_buckets.size())
            {
                m_bucket++;
                while (m_hashmap.m_buckets[m_bucket][m_element_offset].m_next == nullptr)
                    m_element_offset = 0;
                return *this;
            }

            VERIFY_NOT_REACHED();
            return *this;
        }

        constexpr auto& operator*()
        {
            if (m_next != nullptr)
                return *m_next;
            else
                return m_hashmap.m_buckets[m_bucket][m_element_offset];
        }

        constexpr auto const& operator*() const
        {
            if (m_next != nullptr)
                return *m_next;
            else
                return m_hashmap.m_buckets[m_bucket][m_element_offset];
        }

        constexpr bool operator==(HashmapIteratorContainer const& other) const
        {
            return &m_hashmap == &other.m_hashmap && m_bucket == other.m_bucket && m_element_offset == other.m_element_offset && m_next == other.m_next;
        }

        constexpr bool is_end() const
        {
            return m_bucket == m_hashmap.m_buckets.size();
        }

    private:
        THashmap& m_hashmap;

        size_t m_bucket;
        size_t m_element_offset;
        HashmapRecord<TKey, TValue>* m_next;
    };

    template<typename TKey, typename TValue, typename Hasher>
    class Hashmap
    {
        friend HashmapIteratorContainer<Hashmap, TKey, TValue>;

    public:
        using iterator = HashmapIteratorContainer<Hashmap, TKey, TValue>;
        using const_iterator = const HashmapIteratorContainer<Hashmap, TKey, TValue>;

        Hashmap() = delete;

        constexpr Hashmap(size_t initial_bucket_count, size_t initial_bucket_capacity) :
            m_buckets(initial_bucket_count, false), m_colliding_key_storage((size_t)4, false), m_size(0)
        {
            for (size_t i = 0; i < initial_bucket_count; ++i)
                m_buckets.append(Buffer<HashmapRecord<TKey, TValue>>::create_zero_initialized(initial_bucket_capacity));
        }

        constexpr Hashmap(Hashmap const& other) :
            m_buckets(other.m_buckets.capacity(), false), m_colliding_key_storage(other.m_colliding_key_storage.capacity(), false), m_size(other.m_size)
        {
            for (auto& i : other)
                insert(i.m_key, i.m_value);
        }

        constexpr Hashmap(Hashmap&& other) :
            m_buckets(move(other.m_buckets)), m_colliding_key_storage(move(other.m_colliding_key_storage)), m_size(other.m_size)
        {
        }

        constexpr Hashmap& operator=(Hashmap const& other)
        {
            if (this == &other)
                return *this;

            this->~Hashmap();
            new (this) Hashmap(other);

            return *this;
        }

        constexpr Hashmap& operator=(Hashmap&& other)
        {
            if (this == &other)
                return *this;

            this->~Hashmap();
            new (this) Hashmap(move(other));

            return *this;
        }

        constexpr iterator begin()
        {
            return { *this, false };
        }

        constexpr const_iterator begin() const
        {
            return { *this, false };
        }

        constexpr iterator end()
        {
            return { *this, true };
        }

        constexpr const_iterator end() const
        {
            return { *this, true };
        }

        // returns true if it was inserted, or false if it was already in the table
        template<typename TTValue>
        requires Same<RemoveCV<RemoveReference<TTValue>>, TValue>
        constexpr bool insert(TKey const& key, TTValue&& value)
        {
            if (m_size > m_buckets.size() * m_buckets[0].size() * 0.75)
            {
                __builtin_printf("Hashmap size reached %ld, resizing...", m_size);

                Hashmap larger_hashmap(m_buckets.size() * 4, m_buckets.first().size());

                for (auto& i : *this)
                    larger_hashmap.insert(i.m_key, move(i.m_value));

                *this = move(larger_hashmap);
            }

            size_t hash = Hasher::hash(key);
            auto bucket = hash % m_buckets.size();
            auto index = hash % m_buckets[bucket].size();
            auto& hit = m_buckets[bucket][index];

            if (hit.m_next == nullptr)
            {
                new (&hit) HashmapRecord<TKey, TValue> { key, forward<TTValue>(value), (HashmapRecord<TKey, TValue>*)-1 };
                m_size++;
                return true;
            }
            else if (hit.m_next == (HashmapRecord<TKey, TValue>*)-1)
            {
                if (hit.m_key == key)
                    return false;

                if (m_colliding_key_storage.is_empty())
                    m_colliding_key_storage.construct(Bitset<512> { 512, false }, Buffer<HashmapRecord<TKey, TValue>>::create_zero_initialized(512));

                for (auto& p : m_colliding_key_storage)
                {
                    auto& b = p.template get<Bitset<512>>();
                    auto maybe_index = b.find_first_not_set();
                    if (maybe_index.has_value())
                    {
                        new (&p.template get<Buffer<HashmapRecord<TKey, TValue>>>()[maybe_index.value()]) HashmapRecord<TKey, TValue> { key, value, nullptr };
                        b.set(maybe_index.value(), true);
                        m_size++;
                        return true;
                    }
                }
                // at this point, all spill buffers are full. let's create one more
                m_colliding_key_storage.construct(Bitset<512> { 512, false }, Buffer<HashmapRecord<TKey, TValue>>::create_zero_initialized(512));
                m_colliding_key_storage.last().template get<Bitset<512>>().set(0, true);
                new (&m_colliding_key_storage.last().template get<Buffer<HashmapRecord<TKey, TValue>>>()[0]) HashmapRecord<TKey, TValue> { key, value, nullptr };
                m_size++;
                return true;
            }
            else
            {
                auto* next = &hit;
                while (next->m_next != nullptr)
                {
                    if (next->m_key == key)
                        return false;

                    next = next->m_next;
                }

                for (auto& p : m_colliding_key_storage)
                {
                    auto& b = p.template get<Bitset<512>>();
                    auto maybe_index = b.find_first_not_set();
                    if (maybe_index.has_value())
                    {
                        new (&p.template get<Buffer<HashmapRecord<TKey, TValue>>>()[maybe_index.value()]) HashmapRecord<TKey, TValue> { key, value, nullptr };
                        next->m_next = &p.template get<Buffer<HashmapRecord<TKey, TValue>>>()[maybe_index.value()];
                        b.set(maybe_index.value(), true);
                        m_size++;
                        return true;
                    }
                }
                // at this point, all spill buffers are full. let's create one more
                m_colliding_key_storage.construct(Bitset<512> { 512, false }, Buffer<HashmapRecord<TKey, TValue>>::create_zero_initialized(512));
                m_colliding_key_storage.last().template get<Bitset<512>>().set(0, true);
                new (&m_colliding_key_storage.last().template get<Buffer<HashmapRecord<TKey, TValue>>>()[0]) HashmapRecord<TKey, TValue> { key, value, nullptr };
                next->m_next = &m_colliding_key_storage.last().template get<Buffer<HashmapRecord<TKey, TValue>>>()[0];
                m_size++;
                return true;
            }
        }

        constexpr bool remove(TKey const& key)
        {
            size_t hash = Hasher::hash(key);
            auto bucket = hash % m_buckets.size();
            auto index = hash % m_buckets[bucket].size();
            auto& hit = m_buckets[bucket][index];

            if (hit.m_next == nullptr)
            {
                return false;
            }
            else if (hit.m_next == (HashmapRecord<TKey, TValue>*)-1 && hit.m_key == key)
            {
                hit.~HashmapRecord<TKey, TValue>();
                hit.m_next = nullptr;
                return true;
            }
            else
            {
                auto* next = hit.m_next;
                auto* prev = &hit;
                while (next != nullptr)
                {
                    if (next->m_key == key)
                    {
                        prev->m_next = next->m_next;
                        next->~HashmapRecord<TKey, TValue>();
                        next->m_next = nullptr;
                        for (size_t i = 0; i < m_colliding_key_storage.size(); ++i)
                        {
                            auto distance = ((size_t)next - (size_t)m_colliding_key_storage[i].template get<Buffer<HashmapRecord<TKey, TValue>>>().data()) / sizeof(HashmapRecord<TKey, TValue>);
                            if (distance < 512)
                            {
                                m_colliding_key_storage[i].template get<Bitset<512>>().set(distance, false);
                                return true;
                            }
                        }
                    }
                    else
                    {
                        prev = next;
                        next = next->m_next;
                    }
                }
            }
            VERIFY_NOT_REACHED();
        }

        constexpr Optional<ReferenceWrapper<TValue>> get(TKey const& key)
        {
            size_t hash = Hasher::hash(key);
            auto bucket = hash % m_buckets.size();
            auto index = hash % m_buckets[bucket].size();
            auto& hit = m_buckets[bucket][index];

            if (hit.m_next == nullptr)
                return {};
            else
            {
                if (hit.m_key == key)
                    return { hit.m_value };
                else if (hit.m_next != (HashmapRecord<TKey, TValue>*)-1)
                {
                    auto* next = hit.m_next;
                    while (next != nullptr)
                    {
                        if (next->m_key == key)
                            return { next->m_value };
                        else
                            next = next->m_next;
                    }
                    return {};
                }
            }

            return {};
        }

        constexpr Optional<ReferenceWrapper<const TValue>> get(TKey const& key) const
        {
            size_t hash = Hasher::hash(key);
            auto bucket = hash % m_buckets.size();
            auto index = hash % m_buckets[bucket].size();
            auto& hit = m_buckets[bucket][index];

            if (hit.m_next == nullptr)
                return {};
            else
            {
                if (hit.m_key == key)
                    return { hit.m_value };
                else if (hit.m_next != (HashmapRecord<TKey, TValue>*)-1)
                {
                    auto* next = hit.m_next;
                    while (next != nullptr)
                    {
                        if (next->m_key == key)
                            return { next->m_value };
                        else
                            next = next->m_next;
                    }
                    return {};
                }
            }

            return {};
        }

        constexpr bool contains(TKey const& key) const
        {
            return get(key).has_value();
        }

    private:
        Vector<Buffer<HashmapRecord<TKey, TValue>>> m_buckets;
        Vector<Pair<Bitset<512>, Buffer<HashmapRecord<TKey, TValue>>>> m_colliding_key_storage;
        size_t m_size;
    };
}
using neo::Hashmap;
