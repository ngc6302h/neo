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

#include "Assert.h"
#include "Iterator.h"
#include "New.h"
#include "Types.h"

namespace neo
{
    template<typename T>
    class alignas(T) Optional
    {
    public:
        constexpr Optional() = default;

        constexpr ~Optional()
        {
            if (m_has_value)
                ((T*)(m_storage))->~T();
        }

        constexpr Optional(const T& other) :
            m_has_value(true)
        {
            new (m_storage) T(other);
        }

        constexpr Optional(Optional const& other) :
            m_has_value(other.m_has_value)
        {
            if (other.has_value())
            {
                new (this) Optional(other.value());
            }
        }

        constexpr Optional(T&& other) :
            m_has_value(true)
        {
            new (m_storage) T(move(other));
        }

        constexpr Optional(Optional&& other) :
            m_has_value(other.m_has_value)
        {
            if (other.has_value())
            {
                new (this) Optional(other.release_value());
                other.m_has_value = false;
            }
        }

        constexpr Optional& operator=(const Optional& other)
        {
            if (this == &other)
                return *this;

            ~Optional();
            new (this) Optional(other);

            return *this;
        }

        constexpr Optional& operator=(Optional&& other)
        {
            if (this == &other)
                return *this;

            this->~Optional();
            new (this) Optional(move(other));

            return *this;
        }

        constexpr explicit operator bool()
        {
            return m_has_value;
        }

        constexpr explicit operator T&()
        {
            VERIFY(has_value());
            return value();
        }

        constexpr explicit operator T const&() const
        {
            VERIFY(has_value());
            return value();
        }

        [[nodiscard]] constexpr bool has_value() const
        {
            return m_has_value;
        }

        [[nodiscard]] constexpr T& value()
        {
            VERIFY(has_value());
            return *reinterpret_cast<T*>(&m_storage);
        }

        [[nodiscard]] constexpr T const& value() const
        {
            VERIFY(has_value());
            return *reinterpret_cast<T const*>(&m_storage);
        }

        [[nodiscard]] constexpr T release_value()
        {
            VERIFY(has_value());
            return *reinterpret_cast<T*>(&m_storage);
        }

        [[nodiscard]] T& value_or(T& fallback)
        {
            if (m_has_value)
                return value();
            return fallback;
        }

        [[nodiscard]] T const& value_or(T const& fallback) const
        {
            if (m_has_value)
                return value();
            return fallback;
        }

    private:
        u8 m_storage[sizeof(T)] { 0 };
        bool m_has_value { false };
    };

    template<typename T>
    class Optional<ReferenceWrapper<T>>
    {
    public:
        constexpr Optional() = default;
        constexpr ~Optional() = default;

        constexpr Optional(T* other) :
            m_ref(other)
        {
        }

        constexpr Optional(Optional const& other) :
            m_ref(other.m_value)
        {
        }

        constexpr Optional(Optional&& other) :
            m_ref(other.m_value)
        {
            other.m_value = nullptr;
        }

        constexpr Optional& operator=(Optional const& other)
        {
            if (this == &other)
                return *this;

            this->~Optional();
            new (this) Optional(other);

            return *this;
        }

        constexpr Optional& operator=(Optional&& other)
        {
            if (this == &other)
                return *this;

            this->~Optional();
            new (this) Optional(move(other));

            return *this;
        }

        constexpr operator bool()
        {
            return has_value();
        }

        constexpr explicit operator T&()
        {
            VERIFY(has_value());
            return *m_ref;
        }

        constexpr explicit operator T const&() const
        {
            VERIFY(has_value());
            return *m_ref;
        }

        [[nodiscard]] constexpr bool has_value() const
        {
            return m_ref != nullptr;
        }

        [[nodiscard]] constexpr T& value()
        {
            VERIFY(has_value());
            return *m_ref;
        }

        [[nodiscard]] constexpr T const& value() const
        {
            VERIFY(has_value());
            return *m_ref;
        }

        [[nodiscard]] constexpr T& value_or(T& fallback)
        {
            return m_ref != nullptr ? *m_ref : fallback;
        }

        [[nodiscard]] constexpr T const& value_or(T const& fallback) const
        {
            return m_ref != nullptr ? *m_ref : fallback;
        }

    private:
        T* m_ref { nullptr };
    };

    template<typename T>
    class Optional<T*>
    {
    public:
        constexpr Optional() = default;
        constexpr ~Optional() = default;

        constexpr Optional(T* other) :
            m_value(other)
        {
        }

        constexpr Optional(Optional const& other) :
            m_value(other.m_value)
        {
        }

        constexpr Optional(Optional&& other) :
            m_value(other.m_value)
        {
            other.m_value = nullptr;
        }

        constexpr Optional& operator=(const Optional& other)
        {
            if (this == &other)
                return *this;

            this->~Optional();
            new (this) Optional(other);

            return *this;
        }

        constexpr Optional& operator=(Optional&& other)
        {
            if (this == &other)
                return *this;

            this->~Optional();
            new (this) Optional(move(other));

            return *this;
        }

        constexpr operator bool()
        {
            return has_value();
        }

        constexpr explicit operator T*()
        {
            VERIFY(has_value());
            return m_value;
        }

        constexpr explicit operator T const*() const
        {
            VERIFY(has_value());
            return m_value;
        }

        [[nodiscard]] constexpr bool has_value() const
        {
            return m_value != nullptr;
        }

        [[nodiscard]] constexpr T* value()
        {
            VERIFY(has_value());
            return m_value;
        }

        [[nodiscard]] constexpr T const* value() const
        {
            VERIFY(has_value());
            return m_value;
        }

        [[nodiscard]] constexpr T& value_or(T* fallback)
        {
            return m_value != nullptr ? m_value : fallback;
        }

        [[nodiscard]] constexpr T const& value_or(T const* fallback) const
        {
            return m_value != nullptr ? m_value : fallback;
        }

    private:
        T* m_value { nullptr };
    };
}
using neo::Optional;
