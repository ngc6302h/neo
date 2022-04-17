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
#include "Util.h"
#include "TypeTraits.h"
#include "Assert.h"
#include "ResultOrError.h"
#include "Array.h"
#include <sys/mman.h>
#include "New.h"
#include <errno.h>
#include <string.h>

namespace neo
{
    class BasicAllocator
    {
    public:

        BasicAllocator& operator=(BasicAllocator&&) = delete;
        BasicAllocator& operator=(BasicAllocator const&) = delete;

        template<typename T, typename... Args>
        static T* allocate_initialized(size_t n, Args&&... args)
        {
            T* block = reinterpret_cast<T*>(__builtin_malloc(n * sizeof(T)));
            if (block == nullptr)
                return nullptr;
            for (size_t i = 0; i < n; i++)
            {
                new (block + i) T(forward<Args>(args)...);
            }
            return block;
        }

        template<typename T>
        static T* allocate_uninitialized(size_t n)
        {
            return reinterpret_cast<T*>(__builtin_malloc(n * sizeof(T)));
        }

        template<typename T>
        static void deallocate_destroying(T* ptr, size_t n)
        {
            for (size_t i = 0; i < n; ++i)
            {
                (ptr + i)->~T();
            }
            __builtin_free(ptr);
        }

        template<typename T>
        static void deallocate(T* ptr)
        {
            __builtin_free(ptr);
        }
    };

    // Maps a lot of memory but just allocates a portion.
    u8* create_zerocopy_expandable_buffer(size_t initial_size_in_bytes, size_t max_size_in_bytes)
    {
        auto address = mmap(nullptr, max_size_in_bytes, PROT_NONE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (address == MAP_FAILED)
        {
            return nullptr;
        }

        madvise(address, max_size_in_bytes, MADV_HUGEPAGE);

        if (mprotect(address, initial_size_in_bytes, PROT_READ | PROT_WRITE) == 0)
        {
            munmap(address, max_size_in_bytes);
            return nullptr;
        }

        return (u8*)address;
    }

    // Resizes the buffer using the already mapped pages. Specifying a desired size over the original max size results in undefined behaviour.
    void resize_zerocopy_expandable_buffer(u8* base_address, size_t current_size_in_bytes, size_t desired_size_in_bytes)
    {
        mprotect((void*)(base_address + current_size_in_bytes), current_size_in_bytes - desired_size_in_bytes, PROT_READ | PROT_WRITE);
    }

    u8* try_change_max_capacity_zerocopy_expandable_buffer(u8* base_address, size_t old_max_size_in_bytes, size_t new_max_size_in_bytes, bool allow_moving_pages)
    {
        auto result = mremap((void*)base_address, old_max_size_in_bytes, new_max_size_in_bytes, allow_moving_pages ? MREMAP_MAYMOVE : 0);
        if (result == MAP_FAILED)
            return nullptr;
        return (u8*)result;
    }

    void destroy_zerocopy_expandable_buffer(u8* base_address, size_t max_size_in_bytes)
    {
        munmap((void*)base_address, max_size_in_bytes);
    }

    namespace detail
    {
        struct MemoryBlockInfo
        {
            u64 block_size;
            u64 blocks;
            u8* memory;
            u8* ancillary_data;
        };
    }

    class NeoMM
    {
    public:
        explicit NeoMM() :
            m_block_list({})
        {

            auto bookkeeping_memory = create_zerocopy_expandable_buffer(4 * KiB, 16 * MiB);

            if (bookkeeping_memory == MAP_FAILED)
            {
                __builtin_printf("Fatal error: mmap failed with code %d trying to reserve virtual memory: %s.\n", errno, strerror(errno));
                __builtin_trap();
            }
        }

        u8* alloc(u64 bytes, bool zeroed)
        {
            VERIFY(bytes != 0);
            auto index = log2(bytes) - 1;
            auto& block_info = m_block_list[index];
            if (block_info.memory == nullptr)
            {
                // initialize block info
                block_info.blocks = 0;
                block_info.block_size = 2 << (index + 1);

                void* vmem = create_zerocopy_expandable_buffer(16 * block_info.block_size, 1024 * block_info.block_size);
                if (vmem == nullptr)
                    return nullptr;
            }
        }

    private:
        using BlockInfo = detail::MemoryBlockInfo;
        Array<BlockInfo, 40> m_block_list;
    };
}
