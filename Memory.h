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

#include "Types.h"
#include "SmartPtr.h"

namespace neo
{
    struct MallocAllocator
    {
        static void* allocate(size_t byte_count)
        {
            return __builtin_malloc(byte_count);
        }

        static void* allocate_and_zero(size_t byte_count)
        {
            return __builtin_calloc(1, byte_count);
        }

        static void deallocate(void* ptr)
        {
            __builtin_free(ptr);
        }
    };

    template<typename TObject, typename TAllocator = MallocAllocator, typename... TArgs>
    NullableOwnPtr<TObject> create(TArgs&&... args)
    {
        void* storage = TAllocator::allocate(sizeof(TObject));
        if (storage == nullptr)
            return NullableOwnPtr<TObject>(nullptr);

        new (storage) TObject { forward<TArgs>(args)... };
        return NullableOwnPtr<TObject>(static_cast<TObject*>(storage));
    }

    template<typename TObject, typename TAllocator = MallocAllocator, typename... TArgs>
    NullableRefPtr<TObject> create_refcounted(TArgs&&... args)
    {
        return NullableRefPtr<TObject>(create<TObject, TAllocator, TArgs...>(forward<TArgs>(args)...).release());
    }

    template<typename TObject, typename TAllocator = MallocAllocator>
    void destroy(TObject* ptr)
    {
        ptr->~TObject();
        MallocAllocator::deallocate(ptr);
    }
}
using neo::create;
using neo::create_refcounted;
using neo::destroy;
