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
#include "TypeTraits.h"
#include "Assert.h"

namespace neo
{
    template<typename TReturn, typename... TArgs>
    class Function
    {
        struct CallableStorageView
        {
            virtual TReturn operator()(TArgs...) = 0;
            virtual ~CallableStorageView() = default;
            virtual CallableStorageView* clone() = 0;
        };

        template<typename TCallable>
        struct CallableStorage : public CallableStorageView
        {
            CallableStorage(TCallable callable) :
                m_callable(callable) { }
            ~CallableStorage() = default;

            TCallable m_callable;

            virtual TReturn operator()(TArgs... args) override
            {
                if constexpr (IsSame<TReturn, void>)
                    m_callable(forward<TArgs>(args)...);
                else
                    return m_callable(forward<TArgs>(args)...);
            }

            virtual CallableStorage* clone() override
            {
                return new CallableStorage(m_callable);
            }
        };

    public:

        Function()
        {
        }

        Function(Function const& other)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;

            m_callable_ptr = other.m_callable_ptr->clone();
        }

        Function(Function&& other)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;

            m_callable_ptr = other.m_callable_ptr;
            other.m_callable_ptr = m_callable_ptr;
        }

        Function& operator=(Function const& other)
        {
            new (this) Function(other);
        }

        Function& operator=(Function&& other)
        {
            new (this) Function(std::move(other));
        }

        template<CallableWithReturnType<TReturn, TArgs...> Callable>
        Function(Callable const& callable) :
            m_callable_ptr(new CallableStorage<Callable> { callable })
        {
        }

        template<CallableWithReturnType<TReturn, TArgs...> Callable>
        Function& operator=(Callable callable)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;
            m_callable_ptr = new CallableStorage<Callable> { callable };

            return *this;
        }

        TReturn operator()(TArgs... args) const
        {
            VERIFY(m_callable_ptr != nullptr);
            if constexpr (IsSame<TReturn, void>)
                m_callable_ptr->operator()(forward<TArgs>(args)...);
            else
                return m_callable_ptr->operator()(forward<TArgs>(args)...);
        }

        TReturn operator()() const requires(PackSize<TArgs...> == 0)
        {
            VERIFY(m_callable_ptr != nullptr);
            if constexpr (IsSame<TReturn, void>)
                m_callable_ptr->operator()();
            else
                return m_callable_ptr->operator()();
        }

        bool is_valid()
        {
            return m_callable_ptr != nullptr;
        }

    private:
        CallableStorageView* m_callable_ptr { nullptr };
    };

    template<typename TReturn>
    class Function<TReturn, void>
    {
        struct CallableStorageView
        {
            virtual TReturn operator()() = 0;
            virtual ~CallableStorageView() = default;
            virtual CallableStorageView* clone() = 0;
        };

        template<typename TCallable>
        struct CallableStorage : public CallableStorageView
        {
            CallableStorage(TCallable callable) :
                m_callable(callable) { }
            ~CallableStorage() = default;

            TCallable m_callable;

            virtual TReturn operator()() override
            {
                if constexpr (IsSame<TReturn, void>)
                    m_callable();
                else
                    return m_callable();
            }

            virtual CallableStorage* clone() override
            {
                return new CallableStorage(m_callable);
            }
        };

    public:

        Function()
        {
        }

        Function(Function const& other)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;

            m_callable_ptr = other.m_callable_ptr->clone();
        }

        Function(Function&& other)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;

            m_callable_ptr = other.m_callable_ptr;
            other.m_callable_ptr = m_callable_ptr;
        }

        Function& operator=(Function const& other)
        {
            if (this == &other)
                return *this;
            new (this) Function(other);
            return *this;
        }

        Function& operator=(Function&& other)
        {
            if (this == &other)
                return *this;
            new (this) Function(std::move(other));
            return *this;
        }

        template<CallableWithReturnType<TReturn> Callable>
        Function(Callable const& callable) :
            m_callable_ptr(new CallableStorage<Callable> { callable })
        {
        }

        template<CallableWithReturnType<TReturn> Callable>
        Function& operator=(Callable callable)
        {
            if (m_callable_ptr != nullptr)
                delete m_callable_ptr;
            m_callable_ptr = new CallableStorage<Callable> { callable };

            return *this;
        }

        TReturn operator()() const
        {
            VERIFY(m_callable_ptr != nullptr);
            if constexpr (IsSame<TReturn, void>)
                m_callable_ptr->operator()();
            else
                return m_callable_ptr->operator()();
        }

        bool is_valid()
        {
            return m_callable_ptr != nullptr;
        }

    private:
        CallableStorageView* m_callable_ptr { nullptr };
    };
}
using neo::Function;
