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
#include "Preprocessor.h"

#if (__SIZEOF_POINTER__ == 4)
    #define PTR_TYPE u32
#else
    #define PTR_TYPE u64
#endif

template<typename T>
using Aliasing __attribute__((__may_alias__)) = T;

#ifndef NEO_BASIC_TYPES
    #define NEO_BASIC_TYPES

using i8 = __INT8_TYPE__;
using u8 = __UINT8_TYPE__;
using i16 = __INT16_TYPE__;
using u16 = __UINT16_TYPE__;
using i32 = __INT32_TYPE__;
using u32 = __UINT32_TYPE__;
using i64 = __INT64_TYPE__;
using u64 = __UINT64_TYPE__;
using i128 = __int128;
using u128 = unsigned __int128;

using f32 = float;
using f64 = double;

using int8_t = i8;
using uint8_t = u8;
using int16_t = i16;
using uint16_t = u16;
using int32_t = i32;
using uint32_t = u32;
using int64_t = i64;
using uint64_t = u64;

using size_t = __SIZE_TYPE__;
using ptr_t = PTR_TYPE;
#endif

namespace neo
{
    template<typename T>
    struct ReferenceWrapper
    {
        using type = T;
        constexpr ReferenceWrapper(const T& obj) :
            ref(const_cast<T*>(&obj))
        {
        }

        constexpr ReferenceWrapper(ReferenceWrapper const& obj) :
            ref(const_cast<T*>(obj.ref))
        {
        }

        constexpr ~ReferenceWrapper()
        {
            ref = nullptr;
        }

        constexpr T& unwrapped()
        {
            return *ref;
        }

        constexpr T const& unwrapped() const
        {
            return *ref;
        }

        constexpr operator T&()
        {
            return *ref;
        }

        constexpr operator T const&() const
        {
            return *ref;
        }

        constexpr T* operator->()
        {
            return ref;
        }

        constexpr T const* operator->() const
        {
            return ref;
        }

        constexpr ReferenceWrapper& operator=(const ReferenceWrapper& other)
        {
            if (this == &other)
                return *this;
            ref = other.ref;
            return *this;
        };

        constexpr ReferenceWrapper& operator=(T& other)
        {
            ref = &other;
            return *this;
        };

    private:
        T* ref { nullptr };
    };

    template<typename T>
    static constexpr auto DefaultEqualityComparer = [](const T& a, const T& b) constexpr -> bool
    {
        return a == b;
    };

    template<typename T, size_t Size>
    struct initializer_list
    {
        using type = T;
        static constexpr size_t size = Size;

        constexpr ~initializer_list() = default;

        [[nodiscard]] constexpr const T& operator[](size_t index) const
        {
            return m_storage[index];
        }

        [[nodiscard]] constexpr T& operator[](size_t index)
        {
            return m_storage[index];
        }

        [[nodiscard]] constexpr T* data()
        {
            return m_storage;
        }

        [[nodiscard]] constexpr T const* data() const
        {
            return m_storage;
        }

        T m_storage[Size];
    };

    template<typename T>
    constexpr ReferenceWrapper<T> ref(T& obj) { return ReferenceWrapper(obj); }
}
using nullptr_t = decltype(nullptr);

using neo::ReferenceWrapper;

#ifndef NEO_DO_NOT_DEFINE_STD
namespace std
{
    template<typename T, size_t Size>
    using initializer_list = neo::initializer_list<T, Size>;
}
#endif
