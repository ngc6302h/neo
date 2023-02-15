/*
    Copyright (C) 2022  Iori Torres (shortanemoia@protonmail.com)
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "Tuple.h"
#include "Vector.h"
#include "Array.h"

namespace neo
{
    template<typename TKey, typename TValue, size_t Capacity = NumericLimits<u8>::max()>
    class SmallMap
    {
    public:
        SmallMap() = default;

        template<size_t N>
        constexpr SmallMap(Tuple<TKey, TValue>(&&key_value_pairs)[N])
        {
            for(size_t i = 0; i < N; i++)
                insert(std::move(key_value_pairs[i].template get<TKey>()), std::move(key_value_pairs[i].template get<TValue>()));
        }

        template<typename TK, typename TV>
        constexpr void insert(TK&& key, TV&& value)
        {
            VERIFY(m_size != 0xFF);
            size_t index = m_size > 0 ? lower_bound(m_keys.data(), m_size, key) - 1 : 0;
            OverlappingMoveOrCopy(m_size - index, m_keys.data() + index, m_keys.data() + index + 1);
            OverlappingMoveOrCopy(m_size - index, m_value.data() + index, m_value.data() + index + 1);
            m_keys[index] = forward<TKey>(key);
            m_value[index] = forward<TValue>(value);
            ++m_size;
        }

        constexpr Optional<ReferenceWrapper<TValue>> get(TKey const& key) const
        {
            auto maybe_index = bsearch(m_keys.data(), m_size, key);
            if (maybe_index != -1)
                return { ref(m_value[maybe_index]) };

            return {};
        }

        constexpr void remove(TKey const& key)
        {
            VERIFY(m_size > 0);
            size_t index = bsearch(m_keys, m_size, key);
            OverlappingMoveOrCopy(m_size - index - 1, m_keys.data() + index + 1, m_keys.data() + index);
            OverlappingMoveOrCopy(m_size - index - 1, m_value.data() + index + 1, m_value.data() + index);
            --m_size;
        }

        constexpr Array<TKey, Capacity> const& keys() const
        {
            return m_keys;
        }

        constexpr Array<TValue, Capacity> const& values() const
        {
            return m_value;
        }

        constexpr size_t size() const
        {
            return m_size;
        }

        constexpr size_t capacity() const
        {
            return Capacity;
        }

    private:
        Array<TKey, Capacity> m_keys;
        Array<TValue, Capacity> m_value;
        u8 m_size { 0 };
    };

}
using neo::SmallMap;
