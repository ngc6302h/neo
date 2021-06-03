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
#include "New.h"
#include "TypeTraits.h"

namespace neo
{
    template<typename TResult, typename TError>
    class alignas(max(alignof(TResult), alignof(TError))) ResultOrError
    {
    public:
        ResultOrError() = delete;
        ResultOrError& operator=(ResultOrError&& other) = delete;

        constexpr ResultOrError& operator=(const ResultOrError& other)
        {
            if (this == &other)
                return *this;

            if constexpr (IsTriviallyCopyable<TResult> && IsTriviallyCopyable<TError>)
            {
                __builtin_memcpy(m_storage, other.m_storage, sizeof(m_storage));
            }
            else
            {
                if (other.m_has_error)
                    *reinterpret_cast<TError*>(&m_storage) = *reinterpret_cast<TError*>(&other.m_storage);
                else
                    *reinterpret_cast<TResult*>(&m_storage) = *reinterpret_cast<TResult*>(&other.m_storage);
            }
            m_has_error = other.m_has_error;
        }

        constexpr ResultOrError(ResultOrError&& other) :
            m_has_error(other.m_has_error)
        {
            if (other.m_has_error)
                *reinterpret_cast<TError*>(&m_storage) = move(*reinterpret_cast<TError*>(&other.m_storage));
            else
                *reinterpret_cast<TResult*>(&m_storage) = move(*reinterpret_cast<TResult*>(&other.m_storage));
        }

        constexpr ResultOrError(const TResult& other) :
            m_has_error(false)
        {
            new (&m_storage) TResult(other);
        }

        constexpr ResultOrError(TResult&& other) :
            m_has_error(false)
        {
            new (&m_storage) TResult(move(other));
        }

        constexpr ResultOrError(const TError& other) :
            m_has_error(true)
        {
            new (&m_storage) TError(other);
        }

        constexpr ResultOrError(TError&& other) :
            m_has_error(true)
        {
            new (&m_storage) TError(move(other));
        }

        [[nodiscard]] constexpr bool has_error() const
        {
            return m_has_error;
        }

        [[nodiscard]] constexpr bool has_result() const
        {
            return !m_has_error;
        }

        [[nodiscard]] constexpr TResult& result()
        {
            VERIFY(has_result());
            return *reinterpret_cast<TResult*>(&m_storage);
        }

        [[nodiscard]] constexpr TError& error()
        {
            VERIFY(has_error());
            return *reinterpret_cast<TError*>(&m_storage);
        }

        [[nodiscard]] constexpr TResult result_or(TResult alternative) const
        {
            if (m_has_error)
                return alternative;
            return *(TResult*)&m_storage;
        }

    private:
        char m_storage[max(sizeof(TResult), sizeof(TError))] { false };
        bool m_has_error { true };
    };
}
using neo::ResultOrError;
