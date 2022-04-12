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
#include "Concepts.h"
#include "Assert.h"

#if !__has_builtin(__builtin_assume)
    #define __builtin_assume(x)
#endif

namespace neo
{
    template<typename T, size_t Alignment>
    requires(Alignment > 0) class Aligned
    {
    public:
        constexpr Aligned(T value) :
            m_value(value)
        {
            VERIFY(value % Alignment == 0);
        }

        constexpr Aligned& operator=(T value)
        {
            VERIFY(value % Alignment == 0);
            m_value = value;
        }

        constexpr T get() const
        {
            if constexpr (Pointer<T>)
            {
                T v = reinterpret_cast<T>(__builtin_assume_aligned(m_value, Alignment));
                __builtin_assume(reinterpret_cast<size_t>(v) % Alignment == 0);
                return v;
            }
            else
            {
                __builtin_assume(m_value % Alignment == 0);
                return m_value;
            }
        }

        constexpr operator T() const
        {
            if constexpr (Pointer<T>)
            {
                T v = reinterpret_cast<T>(__builtin_assume_aligned(m_value, Alignment));
                __builtin_assume(reinterpret_cast<size_t>(v) % Alignment == 0);
                return v;
            }
            else
            {
                __builtin_assume(m_value % Alignment == 0);
                return m_value;
            }
        }

    private:
        T m_value;
    };
}
using neo::Aligned;
