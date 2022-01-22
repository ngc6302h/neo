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
#include "Concepts.h"
#include "TypeTags.h"
#include "TypeTraits.h"
#include "Types.h"

namespace neo
{
    template<typename T, typename... Ts>
    class Tuple : public Tuple<Ts...> /*, public IContainer<T>, public IFixedSizeContainer<1 + sizeof...(Ts)>*/
    // if it inherited it, the same members would be declared more than once. Declare it here directly instead
    {
    public:
        using first_type = T;
        using type = T;
        using base_type = Tuple<Ts...>;
        template<typename... Types>
        using template_type = neo::Tuple<Types...>;
        template<typename... Us>
        using container_type = Tuple<Us...>;
        template<size_t Index>
        using TypeOfElementAtIndex = TypeOfIndex<Index, T, Ts...>;

        constexpr Tuple(T&& t, Ts&&... ts) :
            Tuple<Ts...>(forward<Ts>(ts)...), m_item(forward<T>(t))
        {
        }

        template<typename U>
        requires TypeContains<U, T, Ts...> && UniqueType<U, T, Ts...>
        [[nodiscard]] constexpr U& get()
        {
            if constexpr (IsSame<T, U>)
                return m_item;
            else
                return Tuple<Ts...>::template get<U>();
        }

        template<typename U>
        requires TypeContains<U, T, Ts...> && UniqueType<U, T, Ts...>
        [[nodiscard]] constexpr const U& get() const
        {
            if constexpr (IsSame<T, U>)
                return m_item;
            else
                return Tuple<Ts...>::template get<U>();
        }

        template<size_t Index>
        requires(Index < 1 + sizeof...(Ts))
            [[nodiscard]] constexpr TypeOfElementAtIndex<Index> get()
        {
            if constexpr (Index == 0)
                return m_item;
            else
                return Tuple<Ts...>::template get<Index - 1>();
        }

        template<size_t Index>
        requires(Index < 1 + sizeof...(Ts))
            [[nodiscard]] constexpr TypeOfElementAtIndex<Index> get() const
        {
            if constexpr (Index == 0)
                return m_item;
            else
                return Tuple<Ts...>::template get<Index - 1>();
        }

        [[nodiscard]] static constexpr size_t size()
        {
            return 1 + sizeof...(Ts);
        }

        template<typename U>
        requires Same<Tuple, U> && InequalityComparable<T>
        [[nodiscard]] constexpr bool operator==(const U& other) const
        {
            if (get<0>() != other.template get<0>())
                return false;
            else
                return *static_cast<const base_type*>(*this) == static_cast<const base_type&>(other);
        }

    private:
        T m_item;
    };

    template<typename T>
    class Tuple<T> : public IFixedSizeContainer<1>
    {
    public:
        using first_type = T;
        using type = T;
        template<typename... Types>
        using template_type = neo::Tuple<Types...>;
        template<typename... U>
        using container_type = Tuple<U...>;
        template<size_t Index>
        using TypeOfElementAtIndex = T;

        constexpr explicit Tuple(T&& t) :
            m_item(forward<T>(t))
        {
        }

        template<typename>
        [[nodiscard]] constexpr T& get()
        {
            return m_item;
        }

        template<typename>
        [[nodiscard]] constexpr const T& get() const
        {
            return m_item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr T& get()
        {
            static_assert(Index == 0, "Index out of range");
            return m_item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr const T& get() const
        {
            static_assert(Index == 0, "Index out of range");
            return m_item;
        }

        [[nodiscard]] static constexpr size_t size()
        {
            return 1;
        }

        template<typename U>
        requires IsSame<Tuple, U> && InequalityComparable<T>
        [[nodiscard]] constexpr bool operator==(const U& other) const
        {
            if (get<0>() != other.template get<0>())
                return false;
            return true;
        }

    private:
        T m_item;
    };

    template<typename T, typename... TRest>
    constexpr Tuple<T, TRest...> make_tuple(T t, TRest... rest)
    {
        return Tuple<T, TRest...>(forward<T>(t), forward<TRest>(rest)...);
    }

    template<typename A, typename B>
    using Pair = Tuple<A, B>;
}
using neo::make_tuple;
using neo::Pair;
using neo::Tuple;
