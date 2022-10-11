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
#include "Assert.h"
#include "Util.h"
#include "New.h"
#include "TypeTraits.h"
#include "Concepts.h"

namespace neo
{
    template<typename TResult, typename TError>
    class alignas(max(alignof(TResult), alignof(TError))) ResultOrError
    {
    public:
        ResultOrError() = delete;

        ~ResultOrError()
        {
            if (m_has_error)
                ((TError*)m_storage)->~TError();
            else
                ((TResult*)m_storage)->~TResult();
        }

        constexpr ResultOrError(ResultOrError const& other) :
            m_has_error(other.m_has_error)
        {
            if (other.m_has_error)
                Copy(1, (TError*)other.m_storage, (TError*)m_storage);
            else
                Copy(1, (TResult*)other.m_storage, (TResult*)m_storage);
        }

        constexpr ResultOrError(ResultOrError&& other) :
            m_has_error(other.m_has_error)
        {
            if (other.m_has_error)
                MoveOrCopy(1, (TError*)other.m_storage, (TError*)m_storage);
            else
                MoveOrCopy(1, (TResult*)other.m_storage, (TResult*)m_storage);
        }

        constexpr ResultOrError& operator=(const ResultOrError& other)
        {
            if (this == &other)
                return *this;

            ~ResultOrError();
            new (this) ResultOrError(other);

            return *this;
        }

        constexpr ResultOrError& operator=(ResultOrError&& other)
        {
            if (this == &other)
                return *this;

            ~ResultOrError();
            new (this) ResultOrError(move(other));

            return *this;
        }

        constexpr ResultOrError(const TResult& other) :
            m_has_error(false)
        {
            new (&m_storage) TResult(other);
        }

        constexpr ResultOrError(TResult&& other) :
            m_has_error(false)
        {
            new (&m_storage) TResult(forward<TResult>(other));
        }

        constexpr ResultOrError(const TError& other) :
            m_has_error(true)
        {
            new (&m_storage) TError(other);
        }

        constexpr ResultOrError(TError&& other) :
            m_has_error(true)
        {
            new (&m_storage) TError(forward<TError>(other));
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
        char m_storage[max(sizeof(TResult), sizeof(TError))] {};
        bool m_has_error {};
    };

    template<typename TError>
    class ResultOrError<void, TError>
    {
    public:
        ResultOrError() = delete;

        ~ResultOrError()
        {
            if constexpr (!IsTrivial<TError>)
            {
                if (m_has_error)
                    ((TError*)&m_storage)->~TError();
            }
        }

        ResultOrError(ResultOrError const& other) :
            m_has_error(other.m_has_error)
        {
            Copy(1, (TError*)other.m_storage, (TError*)m_storage);
        }

        ResultOrError(ResultOrError&& other) :
            m_has_error(other.m_has_error)
        {
            MoveOrCopy(1, (TError*)other.m_storage, (TError*)m_storage);
            __builtin_memset(other.m_storage, 0, sizeof(other.m_storage));
        }

        constexpr ResultOrError& operator=(const ResultOrError& other)
        {
            if (this == &other)
                return *this;

            ~ResultOrError();
            new (this) ResultOrError(other);

            return *this;
        }

        constexpr ResultOrError& operator=(ResultOrError&& other)
        {
            if (this == &other)
                return *this;

            ~ResultOrError();
            new (this) ResultOrError(move(other));

            return *this;
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

        [[nodiscard]] constexpr bool is_result() const
        {
            return !m_has_error;
        }

        [[nodiscard]] constexpr TError& error()
        {
            VERIFY(has_error());
            return *reinterpret_cast<TError*>(&m_storage);
        }

    private:
        char m_storage[sizeof(TError)] {};
        bool m_has_error {};
    };

    template<>
    class ResultOrError<void, void>
    {
    public:
        constexpr ResultOrError() = delete;
        constexpr ~ResultOrError() = default;
        constexpr ResultOrError(ResultOrError&& other) = default;
        constexpr ResultOrError& operator=(ResultOrError&& other) = default;
        constexpr ResultOrError& operator=(const ResultOrError& other) = default;

        constexpr ResultOrError(bool success) :
            m_has_error(!success)
        {
        }

        [[nodiscard]] constexpr bool is_error() const
        {
            return m_has_error;
        }

        [[nodiscard]] constexpr bool is_success() const
        {
            return !m_has_error;
        }

    private:
        bool m_has_error {};
    };

    template<typename TResult, typename TError>
    using ResultOr = ResultOrError<TResult, TError>;
    template<typename TError>
    using Result = ResultOrError<void, TError>;
}
using neo::Result;
using neo::ResultOr;
using neo::ResultOrError;
