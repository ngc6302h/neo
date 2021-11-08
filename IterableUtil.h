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
#include "TypeTraits.h"
//#include "Vector.h"

namespace neo
{
    template<typename T>
    class Vector;
    
    template<IterableContainer TContainer, typename T, typename TComparerFunc>
    requires CallableWithReturnType<TComparerFunc, bool, const typename TContainer::type&, const T&>
    [[nodiscard]] constexpr auto find(const TContainer& where, const T& what, TComparerFunc&& comparer)
    {
        auto begin = where.begin();
        auto end = where.end();
        do
        {
            if (comparer(*begin, what))
                return begin;
        } while (begin++ != end);
        return end;
    }

    template<IterableContainer TContainer, typename T>
    [[nodiscard]] constexpr auto find(const TContainer& where, const T& what)
    {
        return find(where, what, DefaultEqualityComparer<T>);
    }

    template<IterableContainer TContainer, typename T, typename TComparerFunc>
    requires CallableWithReturnType<TComparerFunc, bool, const typename TContainer::type&, const T&>
    [[nodiscard]] constexpr bool contains(const TContainer& where, const T& what, TComparerFunc comparer)
    {
        for (const auto& x : where)
        {
            if (comparer(x, what))
            {
                return true;
            }
        }
        return false;
    }

    template<IterableContainer TContainer, typename T>
    [[nodiscard]] constexpr bool contains(const TContainer& where, const T& what)
    {
        return contains(where, what, DefaultEqualityComparer<T>);
    }

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
    [[nodiscard]] constexpr decltype(auto) zip(const TTupleA& first, const TTupleB& second, const TTuples&... tuples) requires(IsSameValue<size_t, TTupleA::size(), TTuples::size()>&&...)
    {
        return zip(zip(first, second), tuples...);
    }

    template<IterableContainer TContainer, typename TFunc>
    requires Callable<TFunc, typename TContainer::type, typename TContainer::type>
    constexpr TContainer& for_each(TContainer& where, TFunc do_what)
    {
        for (auto& i : where)
        {
            do_what(i);
        }
        return where;
    }

    template<IterableContainer TContainer, typename TFunc>
    requires CallableWithReturnType<TFunc, bool, typename TContainer::type>
    [[nodiscard]] constexpr auto filter(const TContainer& where, TFunc&& selector)
    {
        Vector<RewrapReference<typename TContainer::type>> selected;
        for (auto& i : where)
        {
            if (selector(i))
            {
                selected.append(RewrapReference<typename TContainer::type>(i));
            }
        }
        return selected;
    }
    
    template<IterableContainer TContainer, typename TComparerFunc>
    requires CallableWithReturnType<TComparerFunc, bool, typename TContainer::type const&, typename TContainer::type const&>
    [[nodiscard]] constexpr auto sorted_view(TContainer const& what, TComparerFunc&& comparer)
    {
        Vector<RewrapReference<typename TContainer::type>> view;
        for (auto& i : what)
        {
            view.append(RewrapReference<typename TContainer::type>(i));
        }
        sort(view, comparer);
        return view;
    }

    template<IterableContainer TContainer, typename TSelector>
    requires Callable<TSelector, typename TContainer::type> &&(!IsSame<ReturnType<TSelector, RemoveReferenceWrapper<typename TContainer::type>>, void>)
        [[nodiscard]] constexpr auto select(const TContainer& where, TSelector&& selector)
    {
        Vector<ReferenceWrapper<ReturnType<TSelector, const RemoveReferenceWrapper<typename TContainer::type>&>>> items;
        for (auto& i : where)
        {
            items.construct(selector(i));
        }
        return items;
    }
    
    template<typename TContainer, typename T>
    struct IterableExtensions
    {
        template<typename TPredicate>
        requires CallableWithReturnType<TPredicate, bool, const T&>
        [[nodiscard]] Vector<ReferenceWrapper<T>> filter(TPredicate&& predicate) const
        {
            return neo::filter(*static_cast<const TContainer*>(this), predicate);
        }
        
        template<typename TSelector>
        requires Callable<TSelector, T> &&(!IsSame<ReturnType<TSelector, T>, void>)
        [[nodiscard]] Vector<ReferenceWrapper<ReturnType<TSelector, const T&>>> select(TSelector&& selector) const
        {
            return neo::select(*static_cast<const TContainer*>(this), selector);
        }
        
        template<typename TComparerFunc>
        requires CallableWithReturnType<TComparerFunc, bool, const T&, const T&>
        [[nodiscard]] constexpr bool contains(const T& what, TComparerFunc comparer)
        {
            return neo::contains(*static_cast<TContainer*>(this), what, comparer);
        }
        
        [[nodiscard]] constexpr bool contains(const T& what)
        {
            return neo::contains(*static_cast<TContainer*>(this), what, DefaultEqualityComparer<const T&>);
        }
    
        template<typename TComparerFunc>
        requires CallableWithReturnType<TComparerFunc, bool, const T&, const T&>
        [[nodiscard]] constexpr bool contains(const T& what, TComparerFunc comparer) const
        {
            return neo::contains(*static_cast<const TContainer*>(this), what, comparer);
        }
    
        [[nodiscard]] constexpr bool contains(const T& what) const
        {
            return neo::contains(*static_cast<const TContainer*>(this), what, DefaultEqualityComparer<const T&>);
        }
    
        template<typename TComparerFunc>
        requires CallableWithReturnType<TComparerFunc, bool, T const&, T const&>
        [[nodiscard]] constexpr Vector<RewrapReference<T>> sort(TComparerFunc comparer)
        {
            return neo::sorted_view(*static_cast<TContainer*>(this), comparer);
        }
    
        template<typename TComparerFunc>
        requires CallableWithReturnType<TComparerFunc, bool, T const&, T const&>
        [[nodiscard]] constexpr Vector<RewrapReference<const T>> sort(TComparerFunc comparer) const
        {
            return neo::sorted_view(*static_cast<const TContainer*>(this), comparer);
        }
    };
}

using neo::contains;
using neo::filter;
using neo::find;
using neo::for_each;
using neo::select;
using neo::sort;
using neo::zip;