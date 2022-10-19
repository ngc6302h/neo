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

#include "SmartPtr.h"
#include "Optional.h"

namespace neo
{
    namespace detail
    {
        template<typename T>
        class FutureState
        {
        public:
            FutureState() = default;

            void set_value(T const& value)
            {
                m_object = value;
            }

            void set_value(T&& value)
            {
                m_object = std::move(value);
            }

            void break_promise()
            {
                m_broken = true;
            }

            [[nodiscard]] bool has_value() const
            {
                return m_object.has_value();
            }

            [[nodiscard]] bool is_broken() const
            {
                return m_broken;
            }

            [[nodiscard]] T& value()
            {
                return m_object.value();
            }

            [[nodiscard]] T release_value()
            {
                return m_object.release_value();
            }

        private:
            Optional<T> m_object {};
            bool m_broken { false };
        };
    }

    template<typename T>
    class Future;

    template<typename T>
    class Promise
    {
        using state = RefPtr<detail::FutureState<T>, true>;

    public:
        Promise() :
            m_state(state::make())
        {
        }

        ~Promise()
        {
            if (m_state.is_valid())
            {
                if (!m_state->has_value())
                    m_state->break_promise();
            }
        }

        Promise(Promise&& other) :
            m_state(std::move(other.m_state))
        {
        }

        Promise& operator=(Promise const&) = delete;
        Promise& operator=(Promise&& other)
        {
            if (this == &other)
                return *this;

            new (this) Promise(std::move(other));

            return *this;
        }

        [[nodiscard]] Future<T> get_future()
        {
            return Future<T>(m_state);
        }

        [[nodiscard]] bool is_fulfilled() const
        {
            return m_state->has_value();
        }

        [[nodiscard]] bool is_valid() const
        {
            return !m_state.is_valid();
        }

        void set_value(T const& value)
        {
            VERIFY(!m_state->has_value());
            m_state->set_value(value);
        }

        void set_value(T&& value)
        {
            VERIFY(!m_state->has_value());
            m_state->set_value(std::move(value));
        }

        Promise& operator=(T const& value)
        {
            VERIFY(!m_state->has_value());
            m_state->set_value(value);

            return *this;
        }

        Promise& operator=(T&& value)
        {
            VERIFY(!m_state->has_value());
            m_state->set_value(std::move(value));

            return *this;
        }

    private:
        state m_state;
    };

    template<typename T>
    class Future
    {
        friend Promise<T>;
        using state = RefPtr<detail::FutureState<T>, true>;

    public:
        Future() = delete;
        Future(Future const& other) :
            m_state(other.m_state)
        {
        }

        Future(Future&& other) :
            m_state(std::move(other.m_state))
        {
        }

        Future& operator=(Future const& other)
        {
            if (this == &other)
                return *this;

            new (this) Future(other);

            return *this;
        }

        Future& operator=(Future&& other)
        {
            if (this == &other)
                return *this;

            new (this) Future(std::move(other));

            return *this;
        }

        operator bool() const
        {
            VERIFY(m_state.is_valid());
            return m_state->has_value();
        }

        [[nodiscard]] bool has_value() const
        {
            VERIFY(m_state.is_valid());
            return m_state->has_value();
        }

        [[nodiscard]] bool is_broken() const
        {
            VERIFY(m_state.is_valid());
            return m_state->is_broken();
        }

        [[nodiscard]] T& value()
        {
            wait();
            return m_state->value();
        }

        [[nodiscard]] T const& value() const
        {
            wait();
            return static_cast<T const&>(m_state->value());
        }

        [[nodiscard]] T release_value()
        {
            wait();
            auto value = std::move(m_state->release_value());
            m_state.clear();
            return value;
        }

        void wait() const
        {
            VERIFY(m_state.is_valid());
            while (!m_state->has_value())
                ;
        }

    private:
        explicit Future(state const& state) :
            m_state(state)
        {
        }

        state m_state;
    };
}
using neo::Future;
using neo::Promise;
