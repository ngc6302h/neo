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

        constexpr Tuple(Tuple&&) = default;
        constexpr Tuple(Tuple const&) = default;
        constexpr Tuple& operator=(Tuple&&) = default;
        constexpr Tuple& operator=(Tuple const&) = default;

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
        [[nodiscard]] constexpr U const& get() const
        {
            if constexpr (IsSame<T, U>)
                return m_item;
            else
                return Tuple<Ts...>::template get<U>();
        }

        template<size_t Index>
        requires(Index < 1 + sizeof...(Ts))
            [[nodiscard]] constexpr decltype(auto) get()
        {
            return get<TypeOfElementAtIndex<Index>>();
        }

        template<size_t Index>
        requires(Index < 1 + sizeof...(Ts))
            [[nodiscard]] constexpr decltype(auto) get() const
        {
            return get<TypeOfElementAtIndex<Index>>();
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

        template<typename Type>
        [[nodiscard]] constexpr bool contains_type() const
        {
            return TypeContains<Type, T, Ts...>;
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

        constexpr Tuple(Tuple&&) = default;
        constexpr Tuple(Tuple const&) = default;
        constexpr Tuple& operator=(Tuple&&) = default;
        constexpr Tuple& operator=(Tuple const&) = default;

        template<typename>
        [[nodiscard]] constexpr RemoveReferenceWrapper<T>& get()
        {
            return m_item;
        }

        template<typename>
        [[nodiscard]] constexpr RemoveReferenceWrapper<T> const& get() const
        {
            return m_item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr RemoveReferenceWrapper<T>& get()
        {
            static_assert(Index == 0, "Index out of range");
            return m_item;
        }

        template<size_t Index>
        [[nodiscard]] constexpr RemoveReferenceWrapper<T> const& get() const
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

        template<typename Type>
        [[nodiscard]] constexpr bool contains_type() const
        {
            return Same<Type, T>;
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

    namespace detail
    {
        template<typename TTuple, size_t Index>
        requires(Same<typename TTuple::template TypeOfElementAtIndex<Index>, typename TTuple::template TypeOfElementAtIndex<Index + 1>>) bool tuple_are_equal_internal(TTuple const& tuple)
        {
            if constexpr (Index < TTuple::size() - 1)
                return tuple.template get<Index>() == tuple.template get<Index + 1>() && tuple_are_equal_internal<TTuple, Index + 1>(tuple);
            else
                return tuple.template get<Index>() == tuple.template get<Index + 1>();
        }
    }

    template<typename TTuple>
    bool tuple_are_equal(TTuple const& tuple)
    {
        if constexpr (TTuple::size() == 1)
            return true;
        return detail::tuple_are_equal_internal<TTuple, 0>(tuple);
    }
}
using neo::make_tuple;
using neo::Pair;
using neo::Tuple;
