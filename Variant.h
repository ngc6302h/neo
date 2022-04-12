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
#include "TypeTraits.h"
#include <Concepts.h>
#include "Util.h"

namespace neo
{
    class _VariantTag
    {
    };

    template<typename... _Types>
    class VariantBase
    {
    protected:
        u8 m_storage[max(sizeof(_Types)...)] { 0 };
        size_t m_active_type { -1ull };
    };

    template<typename... Types>
    class alignas(Types...) Variant : public VariantBase<Types...>, public _VariantTag
    {
    private:
        template<size_t I>
        constexpr void destructor_helper(size_t index)
        {
            if constexpr (I < TYPE_COUNT)
            {
                using T = TypeOfIndex<I, Types...>;

                if (I == index)
                {
                    reinterpret_cast<T*>(this->m_storage)->~T();
                }
                else
                {
                    destructor_helper<I + 1>(index);
                }
            }
        }

    public:
        static constexpr size_t TYPE_COUNT = sizeof...(Types);

        ~Variant()
        {
            if (this->m_active_type != (size_t)-1)
                destructor_helper<0>(this->m_active_type);
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr bool check_type_active() const
        {
            return IndexOfType<T, Types...> == this->m_active_type;
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr Variant(const T& other)
        {
            new (&this->m_storage) T(other);
            this->m_active_type = IndexOfType<T, Types...>;
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr Variant(T&& other)
        {
            new (&this->m_storage) T(move(other));
            this->m_active_type = IndexOfType<T, Types...>;
        }

        template<typename T, typename... Args>
        requires(PackContains<T, Types...>) static constexpr Variant construct(Args&&... args)
        {
            return Variant(T(forward<Args>(args)...));
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr T& get()
        {
            [[maybe_unused]] bool valid = this->m_active_type == IndexOfType<T, Types...>; // cannot use a variadic expression in a macro
            VERIFY(valid);
            return *reinterpret_cast<T*>(&this->m_storage);
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr const T& get() const
        {
            [[maybe_unused]] bool valid = this->m_active_type == IndexOfType<T, Types...>; // cannot use a variadic expression in a macro
            VERIFY(valid);
            return *reinterpret_cast<const T*>(&this->m_storage);
        }

        template<typename T>
        requires(PackContains<T, Types...>&& BaseOf<_VariantTag, T>) constexpr Variant& operator=(const T& other)
        {
            destructor_helper<0>(this->m_active_type);
            *reinterpret_cast<T*>(this->m_storage) = *reinterpret_cast<T*>(other.m_storage);
            this->m_active_type = IndexOfType<T, Types...>;
            return *this;
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr Variant& operator=(const T& other)
        {
            destructor_helper<0>(this->m_active_type);
            *reinterpret_cast<T*>(this->m_storage) = other;
            this->m_active_type = IndexOfType<T, Types...>;
            return *this;
        }

        template<typename T>
        requires(PackContains<T, Types...>&& BaseOf<_VariantTag, T>) constexpr Variant& operator=(T&& other)
        {
            destructor_helper<0>(this->m_active_type);
            *reinterpret_cast<T*>(this->m_storage) = move(*reinterpret_cast<T*>(other.m_storage));
            this->m_active_type = IndexOfType<T, Types...>;
            return *this;
        }

        template<typename T>
        requires(PackContains<T, Types...>) constexpr Variant& operator=(const T&& other)
        {
            destructor_helper<0>(this->m_active_type);
            *reinterpret_cast<T*>(this->m_storage) = move(other);
            this->m_active_type = IndexOfType<T, Types...>;
            return *this;
        }

        constexpr size_t index_of_active_type() const
        {
            return this->m_active_type;
        }
    };
}
using neo::Variant;
