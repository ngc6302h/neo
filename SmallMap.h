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
#include "Tuple.h"
#include "Vector.h"
#include "Array.h"

namespace neo
{
    template<typename TKey, typename TValue, size_t Capacity>
    class SmallMap
    {
    public:
        constexpr void insert(TKey&& key, TValue&& value)
        {
            VERIFY(m_size != 0xFF);
            size_t index = lower_bound(m_keys.data(), m_size, value) - 1;
            OverlappingMoveOrCopy(m_size - index, m_keys.data() + index, m_keys.data() + index + 1);
            OverlappingMoveOrCopy(m_size - index, m_value.data() + index, m_value.data() + index + 1);
            m_keys[index] = forward<TKey>(key);
            m_value[index] = forward<TValue>(value);
        }

        constexpr Optional<ReferenceWrapper<TValue>> get(TKey const& key) const
        {
            auto maybe_index = bsearch(m_keys, m_size, key);
            if (maybe_index != -1)
                return { m_value[maybe_index] };

            return {};
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
