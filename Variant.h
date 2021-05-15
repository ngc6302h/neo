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
#include "Types.h"

namespace neo
{
    template<typename... _Types>
    class VariantBase
    {
    protected:
        u8 m_storage[max(sizeof(_Types)...)]  {0};
        size_t m_active_type { -1ull };
    };
    
    template<typename... Types>
    class alignas(Types...) Variant : public VariantBase<Types...>
    {
    public:
        static constexpr size_t TYPE_COUNT = sizeof... (Types);
        
        template<typename T, typename = EnableIf<PackContains<T, Types...>>>
        explicit Variant(const T& other)
        {
            new(&this->m_storage) T(other);
            this->m_active_type = IndexOfType<T, Types...>;
        }
    
        template<typename T, typename = EnableIf<PackContains<T, Types...>>>
        explicit Variant(T&& other)
        {
            new(&this->m_storage) T(move(other));
            this->m_active_type = IndexOfType<T, Types...>;
        }
    
        template<typename T, typename... Args, typename = EnableIf<PackContains<T, Types...>>>
        static constexpr Variant construct(Args&&... args)
        {
            return Variant(T(forward<Args>(args)...));
        }
    
        template<typename T, typename = EnableIf<PackContains<T, Types...>>>
        constexpr T& get()
        {
            [[maybe_unused]] bool valid = this->m_active_type == IndexOfType<T, Types...>; // cannot use a variadic expression in a macro
            VERIFY(valid);
            return *reinterpret_cast<T*>(&this->m_storage);
        }
    
        template<typename T,  typename = EnableIf<PackContains<T, Types...>>>
        constexpr Variant& operator=(const T& other)
        {
            reinterpret_cast<T*>(&this->m_storage)->~T();
            *reinterpret_cast<T*>(&this->m_storage) = *reinterpret_cast<T*>(&other.m_storage);
            return *this;
        }
    
        template<typename T,  typename = EnableIf<PackContains<T, Types...>>>
        constexpr Variant& operator=(T&& other)
        {
            reinterpret_cast<T*>(&this->m_storage)->~T();
            *reinterpret_cast<T*>(&this->m_storage) = move(*reinterpret_cast<T*>(&other.m_storage));
            return *this;
        }
    };
}
using neo::Variant;
