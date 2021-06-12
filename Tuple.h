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
#include "TypeTraits.h"
#include "Types.h"

namespace neo
{
    template<typename T, typename... Ts>
    class Tuple : public Tuple<Ts...>
    {
    public:
        template<size_t Index>
        using TypeOfElementAtIndex = TypeOfIndex<Index, T, Ts...>;

        constexpr explicit Tuple(T&& t, Ts&&... ts) :
            Tuple<Ts...>(forward<Ts>(ts)...)
        {
            item = T(forward<T>(t));
        }

        template<typename U>
        [[nodiscard]] constexpr U& get() requires TypeContains<U, T, Ts...> && UniqueType<U, T, Ts...>
        {
            if constexpr (IsSame<T, U>)
                return item;
            else
                return Tuple<Ts...>::template get<U>();
        }

        template<typename U>
        [[nodiscard]] constexpr const U& get() const requires TypeContains<U, T, Ts...> && UniqueType<U, T, Ts...>
        {
            if constexpr (IsSame<T, U>)
                return item;
            else
                return Tuple<Ts...>::template get<U>();
        }

        template<size_t Index>
        [[nodiscard]] constexpr TypeOfElementAtIndex<Index> get()
        {
            if constexpr (Index == 0)
                return item;
            else
                return Tuple<Ts...>::template get<Index - 1>();
        }

        template<size_t Index>
        [[nodiscard]] constexpr const TypeOfElementAtIndex<Index> get() const
        {
            if constexpr (Index == 0)
                return item;
            else
                return Tuple<Ts...>::template get<Index - 1>();
        }

        [[nodiscard]] static constexpr size_t size()
        {
            return 1 + sizeof...(Ts);
        }

    private:
        T item;
    };

    template<typename T>
    class Tuple<T>
    {
    public:
        template<size_t Index>
        using TypeOfElementAtIndex = T;

        constexpr explicit Tuple(T&& t) :
            item(forward<T>(t))
        {
        }

        template<typename>
        [[nodiscard]] constexpr T& get()
        {
            return item;
        }

        template<typename>
        [[nodiscard]] constexpr const T& get() const
        {
            return item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr T& get()
        {
            static_assert(Index == 0, "Index out of range");
            return item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr const T& get() const
        {
            static_assert(Index == 0, "Index out of range");
            return item;
        }

        [[nodiscard]] static constexpr size_t size()
        {
            return 1;
        }

    private:
        T item;
    };

    template<typename T, typename... TRest>
    Tuple<T, TRest...> make_tuple(T t, TRest... rest)
    {
        return Tuple<T, TRest...>(forward<T>(t), forward<TRest>(rest)...);
    }

    template<typename A, typename B>
    using Pair = Tuple<A, B>;
}
using neo::make_tuple;
using neo::Pair;
using neo::Tuple;
