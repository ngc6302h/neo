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

namespace neo
{
    template<Integral T>
    class Checked
    {
    public:
        constexpr Checked() :
            m_value(), m_has_error(false)
        {
        }

        constexpr Checked(T value) :
            m_value(value), m_has_error(false)
        {
        }

        constexpr T value() const
        {
            VERIFY(!m_has_error);
            return m_value;
        }

        constexpr bool has_overflow() const
        {
            return m_has_error;
        }

        constexpr Checked operator+(Checked const& other)
        {
            Checked result(*this);
            result.m_has_error |= __builtin_add_overflow(m_value, other.m_value, &result.m_value);
            return result;
        }

        constexpr Checked operator-(Checked const& other)
        {
            Checked result(*this);
            result.m_has_error |= __builtin_sub_overflow(m_value, other.m_value, &result.m_value);
            return result;
        }

        constexpr Checked operator*(Checked const& other)
        {
            Checked result(*this);
            result.m_has_error |= __builtin_mul_overflow(m_value, other.m_value, &result.m_value);
            return result;
        }

        constexpr Checked& operator+=(Checked const& other)
        {
            m_has_error |= __builtin_add_overflow(m_value, other.m_value, &m_value);
            return *this;
        }

        constexpr Checked& operator-=(Checked const& other)
        {
            m_has_error |= __builtin_sub_overflow(m_value, other.m_value, &m_value);
            return *this;
        }

        constexpr Checked& operator*=(Checked const& other)
        {
            m_has_error |= __builtin_mul_overflow(m_value, other.m_value, &m_value);
            return *this;
        }

        constexpr Checked& operator++()
        {
            m_has_error |= __builtin_add_overflow(m_value, 1, &m_value);
            return *this;
        }

        constexpr Checked operator++(int)
        {
            Checked current(*this);
            m_has_error |= __builtin_add_overflow(m_value, 1, &m_value);
            return current;
        }

        constexpr Checked& operator--()
        {
            m_has_error |= __builtin_sub_overflow(m_value, 1, &m_value);
            return *this;
        }

        constexpr Checked operator--(int)
        {
            Checked current(*this);
            m_has_error |= __builtin_sub_overflow(m_value, 1, &m_value);
            return current;
        }

    private:
        T m_value;
        bool m_has_error;
    };
}
