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

#include "Concepts.h"
#include "Tuple.h"
#include "Iterator.h"
//#include "Vector.h"

namespace neo
{
    template<Iterable TContainer, typename TComparerFunc>
    requires CallableWithReturnType<TComparerFunc, bool, typename TContainer::type, typename TContainer::type>
    constexpr void sort(TContainer& what, TComparerFunc comparer)
    {
        for (auto& x : what)
        {
            for (auto& y : what)
            {
                if (comparer(x, y))
                {
                    swap(x, y);
                }
            }
        }
    }

    namespace detail
    {
        template<size_t Size, FixedContainer TTupleA, FixedContainer TTupleB, typename... TupleTypes>
        struct _zip
        {
            static constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b, TupleTypes... tuples)
            {
                return _zip<
                    Size - 1,
                    typename TTupleA::base_type, typename TTupleB::base_type, TupleTypes..., Tuple<typename TTupleA::first_type, typename TTupleB::first_type>>::zip(static_cast<typename TTupleA::base_type>(a),
                    static_cast<typename TTupleB::base_type>(b),
                    tuples...,
                    make_tuple<typename TTupleA::first_type, typename TTupleB::first_type>(a.template get<0>(), b.template get<0>()));
            }
        };

        template<typename TTupleA, typename TTupleB, typename... TupleTypes>
        struct _zip<1, TTupleA, TTupleB, TupleTypes...>
        {
            static constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b, TupleTypes... tuples)
            {
                return make_tuple<TupleTypes..., Tuple<typename TTupleA::first_type, typename TTupleB::first_type>>(
                    tuples..., make_tuple(a.template get<0>(), b.template get<0>()));
            }
        };
    }

    template<typename TTupleA, typename TTupleB>
    requires(TTupleA::size() == TTupleA::size())
        [[nodiscard]] constexpr decltype(auto) zip(const TTupleA& a, const TTupleB& b)
    {
        return detail::_zip<TTupleA::size(), TTupleA, TTupleB>::zip(a, b);
    }

    template<typename TTupleA, typename TTupleB, typename... TTuples>
    requires((TTupleA::size() == TTuples::size()) && ...)
        [[nodiscard]] constexpr decltype(auto) zip(const TTupleA& first, const TTupleB& second, const TTuples&... tuples)
    {
        return zip(zip(first, second), tuples...);
    }

    template<IteratorLike TIterator, typename T>
    constexpr bool contains(TIterator begin, TIterator end, T what, auto equality_comparer = DefaultEqualityComparer<typename TIterator::type>)
    {
        if (begin == end)
            return false;

        for (; begin != end; ++begin)
        {
            if (equality_comparer(*begin, what))
                return true;
        }

        return false;
    }

    template<IteratorLike TIterator, typename T>
    constexpr TIterator find(TIterator begin, TIterator end, T what, auto equality_comparer = DefaultEqualityComparer<typename TIterator::type>)
    {
        if (begin == end)
            return end;

        for (; begin != end; ++begin)
        {
            if (equality_comparer(*begin, what))
                return begin;
        }

        return end;
    }

    template<IteratorLike TIterator, typename TAggregate, Callable<TAggregate&, typename TIterator::type> TAggregatorFunc>
    constexpr TAggregate aggregate(TIterator begin, TIterator end, TAggregatorFunc&& aggregator, TAggregate initial_value = {})
    {
        for (; begin != end; ++begin)
            aggregator(initial_value, *begin);

        return initial_value;
    }

    template<IteratorLike TIterator, Callable<typename TIterator::type> TPredicate>
    constexpr bool all(TIterator begin, TIterator end, TPredicate&& predicate)
    {
        for (; begin != end; ++begin)
        {
            if (!predicate(*begin))
                return false;
        }

        return true;
    }

    template<IteratorLike TIterator, Callable<typename TIterator::type> TPredicate>
    constexpr bool any(TIterator begin, TIterator end, TPredicate&& predicate)
    {
        for (; begin != end; ++begin)
        {
            if (predicate(*begin))
                return true;
        }

        return false;
    }

    template<IteratorLike TIterator, CallableWithReturnType<bool, typename TIterator::type> TPredicate>
    constexpr auto skip_while(TIterator begin, TIterator end, TPredicate&& predicate)
    {
        while (begin != end && predicate(begin))
            ++begin;

        return begin;
    }

    template<IteratorLike TIterator, Callable<typename TIterator::type&> TFunc>
    constexpr auto for_all(TIterator begin, TIterator end, TFunc&& func)
    {
        auto copy = begin;
        for (; begin != end; ++begin)
            func(*begin);

        return copy;
    }

    namespace detail
    {
        template<typename TIterator>
        class RangeLimitedIterator
        {
            using type = typename TIterator::type;
            using iterator_type = TIterator;
            using underlying_container_type = typename TIterator::underlying_container_type;

            constexpr RangeLimitedIterator(TIterator iterator, TIterator& end, size_t size, size_t index) :
                m_iterator(iterator), m_end(end), m_size(size), m_index(index)
            {
            }

            constexpr RangeLimitedIterator& operator++()
            {
                ++m_iterator;
                ++m_index;
                return *this;
            }

            constexpr RangeLimitedIterator operator++(int)
            {
                auto copy = *this;
                return ++copy;
            }

            constexpr RangeLimitedIterator& operator--()
            {
                --m_iterator;
                --m_index;
                return *this;
            }

            constexpr RangeLimitedIterator operator--(int)
            {
                auto copy = *this;
                return --copy;
            }

            constexpr decltype(auto) operator*()
            {
                VERIFY(m_index < m_size);
                return m_dereference(*m_iterator);
            }

            constexpr bool operator==(TIterator const& other)
            {
                return m_iterator == other.m_iterator;
            }

            constexpr bool is_end() const
            {
                return m_iterator == m_end || m_size >= m_index;
            }

        private:
            constexpr decltype(auto) implementation()
            {
                return m_iterator.implementation();
            }

        private:

            TIterator m_iterator;
            TIterator& m_end;
            size_t m_size;
            size_t m_index;
        };
    }

    template<IteratorLike TIterator, Callable<TIterator> TDereferenceFunc, Callable<TIterator> TIncrementFunc, Callable<TIterator> TDecrementFunc>
    class LazyIteratorWrapper
    {
    public:
        using type = ReturnType<TDereferenceFunc, typename TIterator::type>;
        using iterator_type = TIterator;
        using underlying_container_type = typename TIterator::underlying_container_type;

        constexpr LazyIteratorWrapper(TIterator iterator, TIterator& end, TDereferenceFunc&& dereference,
            TIncrementFunc&& increment, TDecrementFunc&& decrement) :
            m_iterator(iterator), m_end(end), m_dereference(dereference), m_increment(increment), m_decrement(decrement)
        {
        }

        constexpr LazyIteratorWrapper& operator++()
        {
            m_increment(m_iterator);
            return *this;
        }

        constexpr LazyIteratorWrapper operator++(int)
        {
            auto copy = *this;
            return ++copy;
        }

        constexpr LazyIteratorWrapper& operator--()
        {
            m_decrement(m_iterator);
            return *this;
        }

        constexpr LazyIteratorWrapper operator--(int)
        {
            auto copy = *this;
            return --copy;
        }

        constexpr decltype(auto) operator*()
        {
            return m_dereference(*m_iterator);
        }

        constexpr bool operator==(TIterator const& other) const
        {
            return m_iterator == other.m_iterator;
        }

        constexpr bool is_end() const
        {
            return m_iterator == m_end;
        }

    private:
        constexpr decltype(auto) implementation()
        {
            return m_iterator.implementation();
        }

        TIterator m_iterator;
        TIterator& m_end;
        TDereferenceFunc m_dereference;
        TIncrementFunc m_increment;
        TDecrementFunc m_decrement;
    };

    template<IteratorLike TIterator>
    class IterableCollection
    {
    public:
        using type = typename TIterator::type;
        using iterator_type = TIterator;
        using underlying_container_type = typename TIterator::underlying_container_type;

        constexpr IterableCollection() = delete;

        template<IterableContainer T>
        constexpr IterableCollection(T& container) :
            m_begin(container.begin()), m_end(container.end())
        {
        }

        constexpr IterableCollection(TIterator const& begin, TIterator const& end) :
            m_begin(begin), m_end(end)
        {
        }

        constexpr TIterator begin()
        {
            return m_begin;
        }

        constexpr const TIterator begin() const
        {
            return m_begin;
        }

        constexpr TIterator end()
        {
            return m_end;
        }

        constexpr const TIterator end() const
        {
            return m_end;
        }

        constexpr size_t size() const
        {
            auto begin = m_begin;
            size_t counter = 0;

            while (begin++ != m_end)
                ++counter;

            return counter;
        }

        template<typename T, CallableWithReturnType<bool, type, T> TComparer>
        constexpr bool contains(T const& what, TComparer equality_comparer)
        {
            return neo::contains(begin(), end(), what, equality_comparer);
        }

        constexpr bool contains(type const& what)
        {
            return neo::contains(begin(), end(), what, DefaultEqualityComparer<type>);
        }

        template<typename T, CallableWithReturnType<bool, type, T> TComparer>
        constexpr auto find(T const& what, TComparer equality_comparer = DefaultEqualityComparer<type>)
        {
            return neo::find(begin(), end(), what, equality_comparer);
        }

        constexpr auto find(type const& what)
        {
            return neo::find(begin(), end(), what, DefaultEqualityComparer<type>);
        }

        template<Callable<type> TPredicate>
        constexpr bool all(TPredicate&& predicate)
        {
            return neo::all(begin(), end(), forward<TPredicate>(predicate));
        }

        template<Callable<type> TPredicate>
        constexpr bool any(TPredicate&& predicate)
        {
            return neo::any(begin(), end(), forward<TPredicate>(predicate));
        }

        template<typename TAggregate, Callable<TAggregate&, type> TAggregatorFunc>
        constexpr TAggregate aggregate(TAggregatorFunc&& aggregator, TAggregate initial_value = {})
        {
            return neo::aggregate(begin(), end(), forward<TAggregatorFunc>(aggregator), initial_value);
        }

        template<CallableWithReturnType<bool, type> TPredicate>
        constexpr auto filter(TPredicate&& predicate)
        {
            constexpr auto increment = [](TIterator & iterator, TPredicate && pred) constexpr
            {
                while (iterator.is_end())
                {
                    ++iterator;
                    if (pred(*iterator))
                        return;
                }
            };

            constexpr auto decrement = [this](TIterator & iterator, TPredicate && pred) constexpr
            {
                while (iterator.is_end() && iterator != m_begin)
                {
                    --iterator;
                    if (pred(*iterator))
                        return;
                }
            };

            return IterableCollection {
                LazyIteratorWrapper { m_begin, m_end, default_dereference, increment, decrement },
                LazyIteratorWrapper { m_end, m_end, default_dereference, increment, decrement }
            };
        }

        template<Callable<typename TIterator::type> TSelectorFunc>
        constexpr auto select(TSelectorFunc&& selector)
        {
            return IterableCollection { LazyIteratorWrapper { m_begin, m_end, forward<TSelectorFunc>(selector), default_increment, default_decrement },
                LazyIteratorWrapper { m_end, m_end, forward<TSelectorFunc>(selector), default_increment, default_decrement } };
        }

        template<Callable<type&> TFunction>
        constexpr auto for_all(TFunction&& function)
        {
            neo::for_all(begin(), end(), forward<TFunction>(function));
            return *this;
        }

        constexpr auto take(size_t n)
        {
            return IterableCollection {
                detail::RangeLimitedIterator { m_begin, m_end, 0 },
                detail::RangeLimitedIterator { skip(m_begin, n), m_end, n }
            };
        }

        constexpr auto skip(size_t n)
        {
            return IterableCollection { skip(m_begin, n), m_end };
        }

        template<CallableWithReturnType<bool, type> TPredicate>
        constexpr auto skip_while(TPredicate&& predicate)
        {
            return IterableCollection { skip_while(begin(), end(), forward<TPredicate>(predicate)), m_end };
        }

        constexpr auto skip_backwards(size_t n)
        {
            return IterableCollection { m_begin, rewind(m_end, n) };
        }

        template<CallableWithReturnType<bool, type> TPredicate>
        constexpr auto skip_backwards_while(TPredicate&& predicate)
        {
            auto end = m_end;
            while (predicate(end))
                --end;

            return IterableCollection { m_begin, end };
        }

    private:
        static constexpr auto identity = [](auto& v) constexpr { return v; };
        static constexpr auto default_dereference = [](auto& v) constexpr { return *v; };
        static constexpr auto default_increment = [](auto& v) constexpr -> auto& { return ++v; };
        static constexpr auto default_decrement = [](auto& v) constexpr -> auto& { return --v; };

        TIterator m_begin;
        TIterator m_end;
    };

    template<typename TContainer, typename T>
    struct IterableExtensions
    {
        constexpr auto to_iterable_collection()
        {
            return IterableCollection<typename TContainer::iterator>(static_cast<TContainer&>(*this).begin(), static_cast<TContainer&>(*this).end());
        }
    };

}

using neo::aggregate;
using neo::all;
using neo::any;
using neo::contains;
using neo::find;
using neo::for_all;
using neo::sort;
using neo::zip;
