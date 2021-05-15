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
#include "Optional.h"
#include "Types.h"
#include "Vector.h"

namespace neo
{
template <typename T>
struct DefaultHasher
{
    static constexpr size_t hash(const T& value)
    {
        u8* data = (u8*)&value;
        size_t size = sizeof(T);
        size_t result = data[size - 1];
        while (size--)
            result += result ^ data[size] ^ (~(result * result + 3241));
        return result;
    }
};

template <typename TKey, typename TValue>
struct KeyValuePair
{
    TKey key;
    TValue value;

    KeyValuePair& operator=(const KeyValuePair&) = default;
    KeyValuePair& operator=(KeyValuePair&&) = default;
};

template <typename TKey, typename TValue, typename Hasher = DefaultHasher<TKey>>
class Hashmap
{
public:
    static constexpr size_t DEFAULT_BUCKET_COUNT = 31; //nearest prime to 32
    static constexpr size_t DEFAULT_BUCKET_CAPACITY = 8;

    constexpr explicit Hashmap(size_t bucket_count = DEFAULT_BUCKET_COUNT, size_t bucket_capacity = DEFAULT_BUCKET_CAPACITY)
        : m_buckets(bucket_count)
        , m_bucket_capacity(bucket_capacity)
        , m_bucket_count(bucket_count)
    {
        for (size_t i = 0; i < bucket_count; i++)
            m_buckets.construct(bucket_capacity, false);
    }

    constexpr Hashmap& operator=(const Hasher& other)
    {
        if (this == &other)
            return *this;

        m_buckets = other.m_buckets;
        m_bucket_count = other.m_bucket_count;
        m_bucket_capacity = other.m_bucket_capcity;
        return *this;
    }

    constexpr Hashmap& operator=(Hasher&& other)
    {
        if (this == &other)
            return *this;

        m_buckets = move(other.m_buckets);
        m_bucket_count = other.m_bucket_count;
        m_bucket_capacity = other.m_bucket_capacity;
        other.m_bucket_count = 0;
        other.m_bucket_capacity = 0;
        return *this;
    }

    constexpr void insert(const TKey& key, const TValue& value)
    {
        size_t index = Hasher::hash(key) % m_bucket_count;
        if (m_buckets[index].size() > DEFAULT_BUCKET_CAPACITY)
            rehash();
        m_buckets[Hasher::hash(key) % m_bucket_count].append({ key, value });
    }

    [[nodiscard]] constexpr Optional<ReferenceWrapper<TValue>> getref(const TKey& key) const
    {
        size_t index = Hasher::hash(key) % m_bucket_count;
        auto it = m_buckets[index].begin();
        auto end = m_buckets[index].end();
        while ((*it).key != key && it != end)
            it++;
        return it != end ? Optional<ReferenceWrapper<TValue>>(ref((*it).value)) : Optional<ReferenceWrapper<TValue>>();
    }

    [[nodiscard]] constexpr Optional<TValue> get(const TKey& key) const
    {
        size_t index = Hasher::hash(key) % m_bucket_count;
        auto it = m_buckets[index].begin();
        auto end = m_buckets[index].end();
        while ((*it).key != key && it != end)
            it++;
        return it != end ? (*it).value : Optional<TValue>();
    }

    [[nodiscard]] constexpr bool contains(const TKey& key) const
    {
        return get(key).has_value();
    }

private:
    constexpr void rehash()
    {
        m_bucket_count *= 2;
        Vector<Vector<KeyValuePair<TKey, TValue>>> new_buckets(m_bucket_count);
        for (size_t i = 0; i < m_bucket_count; i++)
            new_buckets.construct(m_bucket_capacity);
        for (auto& bucket : m_buckets)
        {
            for (auto& pair : bucket)
            {
                new_buckets[Hasher::hash(pair.key) % m_bucket_count].append({ move(pair.key), move(pair.value) });
            }
        }
    }

    Vector<Vector<KeyValuePair<TKey, TValue>>> m_buckets;
    size_t m_bucket_capacity;
    size_t m_bucket_count;
};
}
using neo::Hashmap;
