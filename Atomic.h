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
#include "Util.h"
#include "Concepts.h"

namespace neo
{
    enum MemoryOrder
    {
        Relaxed = __ATOMIC_RELAXED,
        Consume = __ATOMIC_CONSUME,
        Acquire = __ATOMIC_ACQUIRE,
        Release = __ATOMIC_RELEASE,
        AcquireRelease = __ATOMIC_ACQ_REL,
        SequentiallyConsistent = __ATOMIC_SEQ_CST
    };

    template<typename T>
    class Atomic
    {

    public:
        constexpr Atomic& operator=(Atomic const&) = delete;

        constexpr Atomic() = default;
        constexpr Atomic(T const& other)
        {
            m_value = other;
        }

        constexpr operator T() const
        {
            return load(Acquire);
        }

        constexpr void store(T const& value, MemoryOrder order)
        {
            __atomic_store_n(&m_value, value, order);
        }

        constexpr T load(MemoryOrder order) const
        {
            return __atomic_load_n(&m_value, order);
        }

        constexpr T exchange(T value, MemoryOrder order)
        {
            return __atomic_exchange_n(&m_value, value, order);
        }

        constexpr bool compare_exchange_strong(T& expected, T const& desired, MemoryOrder success, MemoryOrder failure)
        {
            return __atomic_compare_exchange_n(&m_value, &expected, desired, false, success, failure);
        }

        constexpr bool compare_exchange_weak(T& expected, T const& desired, MemoryOrder success, MemoryOrder failure)
        {
            return __atomic_compare_exchange_n(&m_value, &expected, desired, true, success, failure);
        }

        constexpr bool is_lock_free() const
        {
            return __atomic_always_lock_free(sizeof(T), nullptr);
        }

        constexpr T fetch_add(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_add(&m_value, value, order);
        }

        constexpr T fetch_sub(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_sub(&m_value, value, order);
        }

        constexpr T fetch_and(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_and(&m_value, value, order);
        }

        constexpr T fetch_xor(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_xor(&m_value, value, order);
        }

        constexpr T fetch_or(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_or(&m_value, value, order);
        }

        constexpr T fetch_nand(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_nand(&m_value, value, order);
        }

        constexpr T add_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_add_fetch(&m_value, value, order);
        }

        constexpr T sub_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_sub_fetch(&m_value, value, order);
        }

        constexpr T and_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_add(&m_value, value, order);
        }

        constexpr T xor_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_add(&m_value, value, order);
        }

        constexpr T or_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_add(&m_value, value, order);
        }

        constexpr T nand_fetch(T const& value, MemoryOrder order)
        {
            return __atomic_fetch_add(&m_value, value, order);
        }

        constexpr T test_and_set(MemoryOrder order) requires(sizeof(T) == 1)
        {
            return __atomic_test_and_set(&m_value, order);
        }

        constexpr T clear(MemoryOrder order) requires(sizeof(T) == 1)
        {
            return __atomic_clear(&m_value, order);
        }

        constexpr T* ptr() __attribute__((always_inline))
        {
            return &m_value;
        }

    private:

        T m_value {};
    };
}
using neo::Atomic;
using neo::MemoryOrder;
