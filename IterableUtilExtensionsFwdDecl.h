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
#include "TypeTraits.h"

namespace neo
{
    //forward declarations
    template<typename T>
    class Vector;

    template<IterableContainer TContainer, typename TFunc>
    [[nodiscard]] /*constexpr*/ Vector<ReferenceWrapper<RemoveReferenceWrapper<typename TContainer::type>>>
    filter(const TContainer& where, TFunc&& selector) requires CallableWithReturnType<TFunc, bool, typename TContainer::type>;

    template<IterableContainer TContainer, typename TFunc>
    constexpr TContainer& for_each(TContainer& where, TFunc do_what) requires Callable<TFunc, typename TContainer::type, typename TContainer::type>;

    template<IterableContainer TContainer, typename TSelector>
    requires Callable<TSelector, typename TContainer::type> &&(!IsSame<ReturnType<TSelector, RemoveReferenceWrapper<typename TContainer::type>>, void>)
        [[nodiscard]] Vector<ReferenceWrapper<ReturnType<TSelector, const RemoveReferenceWrapper<typename TContainer::type>&>>> select(const TContainer& where, TSelector&& selector);

    template<IterableContainer TContainer, typename T, typename TComparerFunc>
    requires CallableWithReturnType<TComparerFunc, bool, const typename TContainer::type&, const typename TContainer::type&>
    [[nodiscard]] constexpr bool contains(const TContainer& where, const T& what, TComparerFunc comparer);

    template<IterableContainer TContainer, typename T>
    [[nodiscard]] constexpr bool contains(const TContainer& where, const T& what);

    //end forward declarations

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
            return neo::contains(*static_cast<const TContainer*>(this), what, comparer);
        }

        [[nodiscard]] constexpr bool contains(const T& what)
        {
            return neo::contains(*static_cast<const TContainer*>(this), what, DefaultEqualityComparer<const T&>);
        }
    };
}