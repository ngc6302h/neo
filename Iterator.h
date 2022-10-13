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

namespace neo
{

    namespace detail
    {
        template<typename T>
        concept IteratorImplementationHasIndexMethod = requires(T t)
        {
            {
                t.index()
                } -> Same<size_t>;
        };

        template<typename T>
        concept IteratorImplementationHasIsEndMethod = requires(T t)
        {
            {
                t.is_end()
                } -> Same<bool>;
        };

        template<typename TImplementation, typename TContainer>
        concept IteratorImplementationHasConstructorThatTakesAContainer = requires(TContainer t)
        {
            TImplementation(t);
        };

        template<typename TImplementation, typename TContainer>
        concept IteratorImplementationHasConstructorThatTakesAContainerAndIndex = requires(TContainer t, size_t index)
        {
            TImplementation(t, index);
        };

    }

    template<typename T, typename U>
    class Iterator;

    template<typename TContainer>
    class DefaultIteratorImplementation
    {
        friend Iterator<TContainer, DefaultIteratorImplementation>;

        constexpr DefaultIteratorImplementation(TContainer& other) :
            m_container(other),
            m_index(0)
        {
        }

        constexpr DefaultIteratorImplementation(TContainer& other, size_t index) :
            m_container(other),
            m_index(index)
        {
        }

        constexpr DefaultIteratorImplementation(DefaultIteratorImplementation const& other) :
            m_container(other.m_container), m_index(other.m_index)
        {
        }

        constexpr DefaultIteratorImplementation& operator=(DefaultIteratorImplementation const& other)
        {
            m_index = other.m_index;
            m_container = other.m_container;
            return *this;
        }

        constexpr DefaultIteratorImplementation operator++(int)
        {
            VERIFY(m_index < m_container.size());
            auto tmp = *this;
            m_index++;
            return tmp;
        }

        constexpr DefaultIteratorImplementation operator--(int)
        {
            VERIFY(m_index > 0);
            auto tmp = *this;
            m_index--;
            return tmp;
        }

        constexpr DefaultIteratorImplementation& operator++()
        {
            VERIFY(m_index < m_container.size());
            m_index++;
            return *this;
        }

        constexpr DefaultIteratorImplementation& operator--()
        {
            VERIFY(m_index > 0);
            m_index--;
            return *this;
        }

        constexpr decltype(auto) operator*()
        {
            VERIFY(!is_end());
            return m_container[m_index];
        }

        constexpr decltype(auto) get()
        {
            VERIFY(!is_end());
            return m_container[m_index];
        }

        constexpr size_t index() const
        {
            return m_index;
        }

        constexpr bool is_end() const
        {
            return m_index >= m_container.size();
        }

        constexpr bool operator==(const DefaultIteratorImplementation& other) const
        {
            return m_index == other.m_index;
        }

        constexpr bool operator!=(const DefaultIteratorImplementation& other) const
        {
            return m_index != other.m_index;
        }

        TContainer& m_container;
        size_t m_index;
    };

    template<typename TContainer, typename TIteratorImplementation = DefaultIteratorImplementation<TContainer>>
    class Iterator
    {
        template<IteratorLike TIterator>
        friend class IterableCollection;

    public:
        using type = typename TContainer::type;
        using underlying_container_type = TContainer;

        Iterator() {};

        constexpr Iterator(TIteratorImplementation const& impl) requires detail::IteratorImplementationHasConstructorThatTakesAContainer<TIteratorImplementation, TContainer> : m_impl(impl)
        {
        }

        constexpr Iterator(TIteratorImplementation&& impl) requires detail::IteratorImplementationHasConstructorThatTakesAContainerAndIndex<TIteratorImplementation, TContainer> : m_impl(std::move(impl))
        {
        }

        constexpr Iterator(TContainer const& container) :
            m_impl(const_cast<TContainer&>(container))
        {
        }

        constexpr Iterator(TContainer const& container, size_t index) :
            m_impl(const_cast<TContainer&>(container), index)
        {
        }

        constexpr Iterator(Iterator const& other) :
            m_impl(other.m_impl)
        {
        }

        constexpr Iterator(Iterator&& other) :
            m_impl(std::move(other.m_impl))
        {
        }

        constexpr Iterator& operator=(Iterator const& other)
        {
            if (this == &other)
                return *this;

            m_impl = other.m_impl;
            return *this;
        }

        constexpr bool operator!=(Iterator const& other) const requires InequalityComparable<TIteratorImplementation>
        {
            return this->m_impl != other.m_impl;
        }

        constexpr bool operator==(Iterator const& other) const requires EqualityComparable<TIteratorImplementation>
        {
            return this->m_impl == other.m_impl;
        }

        constexpr decltype(auto) operator*()
        {
            return *this->m_impl;
        }

        constexpr decltype(auto) operator*() const
        {
            return *this->m_impl;
        }

        constexpr decltype(auto) operator->()
        {
            return &*this->m_impl;
        }

        constexpr decltype(auto) operator->() const
        {
            return &*this->m_impl;
        }

        // To be implemented by the class using iterators
        constexpr Iterator& operator++() requires PrefixIncrementable<TIteratorImplementation>
        {
            ++this->m_impl;
            return *this;
        }
        constexpr Iterator operator++(int) requires PrefixIncrementable<TIteratorImplementation>
        {
            auto prev = *this;
            ++this->m_impl;
            return prev;
        }
        constexpr Iterator& operator--() requires PrefixDecrementable<TIteratorImplementation>
        {
            --this->m_impl;
            return *this;
        }
        constexpr Iterator operator--(int) requires PrefixDecrementable<TIteratorImplementation>
        {
            auto prev = *this;
            ++this->m_impl;
            return prev;
        }

        constexpr size_t index() const requires detail::IteratorImplementationHasIndexMethod<TIteratorImplementation>
        {
            return m_impl.index();
        }

        constexpr bool is_end() const requires detail::IteratorImplementationHasIsEndMethod<TIteratorImplementation>
        {
            return m_impl.is_end();
        }

    private : constexpr TIteratorImplementation& implementation()
        {
            return m_impl;
        }

        TIteratorImplementation m_impl;
    };

    template<IteratorLike T>
    constexpr auto skip(T iterator, size_t n)
    {
        while (n--)
            ++iterator;
        return iterator;
    }

    template<IteratorLike T>
    constexpr auto rewind(T iterator, size_t n)
    {
        while (n--)
            --iterator;
        return iterator;
    }
}

using neo::Iterator;
using neo::rewind;
using neo::skip;
