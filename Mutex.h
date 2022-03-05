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
#include "Atomic.h"
#include "Optional.h"
#include <Concepts.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <syscall.h>
#include <linux/futex.h>
#include <errno.h>

namespace neo
{
    class SpinlockMutex
    {
    public:
        constexpr SpinlockMutex() = default;
        constexpr SpinlockMutex& operator=(SpinlockMutex&) = delete;
        constexpr SpinlockMutex& operator=(SpinlockMutex&&) = delete;

        constexpr ~SpinlockMutex()
        {
            VERIFY(m_control.load(Relaxed) == 0);
        }

        constexpr void lock()
        {
            u32 expected = 0;
            while (!m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire))
                ;
        }

        constexpr bool try_lock()
        {
            u32 expected = 0;
            return m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire);
        }

        constexpr bool unlock()
        {
            u32 expected = 1;
            bool result = m_control.compare_exchange_strong(expected, 0, AcquireRelease, Acquire);
            return result;
        }

        constexpr bool is_locked() const
        {
            return m_control.load(Acquire) == 1;
        }

    private:
        Atomic<u32> m_control { 0 };
    };

    class Mutex
    {
    public:
        constexpr Mutex() = default;
        constexpr Mutex& operator=(Mutex&) = delete;
        constexpr Mutex& operator=(Mutex&&) = delete;

        constexpr ~Mutex()
        {
            VERIFY(m_control.load(Relaxed) == 0);
        }

        constexpr void lock()
        {
            u32 expected = 0;
            while (!m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire))
            {
                [[maybe_unused]] auto result = syscall(SYS_futex, m_control.ptr(), FUTEX_WAIT_PRIVATE, expected, nullptr);
                VERIFY(result != -1);
                expected = 0;
            }
        }

        // true if the lock was acquired
        constexpr bool try_lock()
        {
            u32 expected = 0;
            return m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire) == true;
        }

        constexpr void unlock()
        {
            u32 expected = 1;
            [[maybe_unused]] auto success = m_control.compare_exchange_strong(expected, 0, AcquireRelease, Acquire);
            VERIFY(success)
            success = syscall(SYS_futex, m_control.ptr(), FUTEX_WAKE_PRIVATE, 1) != -1;
            VERIFY(success);
        }

        constexpr bool is_locked() const
        {
            return m_control.load(Acquire) == 1;
        }

    private:
        Atomic<u32> m_control alignas(4);
    };

    class HybridMutex
    {
    public:
        constexpr HybridMutex() = default;
        constexpr HybridMutex& operator=(HybridMutex&) = delete;
        constexpr HybridMutex& operator=(HybridMutex&&) = delete;

        constexpr ~HybridMutex()
        {
            VERIFY(m_control.load(Relaxed) == 0);
        }

        constexpr void lock()
        {

            u8 iterations = 0;
            u32 expected = 0;
            while (!m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire))
            {
                if (++iterations > 60)
                {
                    [[maybe_unused]] auto result = syscall(SYS_futex, m_control.ptr(), FUTEX_WAIT_PRIVATE, 1, nullptr);
                    VERIFY(result != -1);
                    expected = 0;
                }
            }
        }

        // true if the lock was acquired
        constexpr bool try_lock()
        {
            u32 expected = 0;
            return m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire) == true;
        }

        constexpr void unlock()
        {
            u32 expected = 1;
            [[maybe_unused]] auto success = m_control.compare_exchange_strong(expected, 0, AcquireRelease, Acquire);
            VERIFY(success)
            success = syscall(SYS_futex, m_control.ptr(), FUTEX_WAKE_PRIVATE, 1) != -1;
            VERIFY(success);
        }

        constexpr bool is_locked() const
        {
            return m_control.load(Acquire) == 1;
        }

    private:
        Atomic<u32> m_control { 0 };
    };

    class RecursiveMutex
    {
    public:
        RecursiveMutex() = default;
        RecursiveMutex& operator=(RecursiveMutex const&) = delete;
        RecursiveMutex& operator=(RecursiveMutex&&) = delete;

        ~RecursiveMutex()
        {
            VERIFY(m_control.load(Relaxed) == 0);
        }

        u32 lock()
        {
            auto tid = syscall(__NR_gettid);
            if (m_tid == tid)
            {
                return m_control.add_fetch(1, Relaxed);
            }
            else
            {
                u32 expected = 0;
                while (!m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire))
                {
                    [[maybe_unused]] auto result = syscall(SYS_futex, m_control.ptr(), FUTEX_WAIT_PRIVATE, expected, nullptr);
                    VERIFY(result != -1);
                    expected = 0;
                }
                m_tid = tid;
                return 1;
            }
        }

        ssize_t try_lock()
        {
            auto tid = syscall(__NR_gettid);
            if (m_tid == tid)
            {
                return m_control.add_fetch(1, Relaxed);
            }
            else
            {
                constexpr size_t expected = 0;
                bool success = syscall(SYS_futex, m_control.ptr(), FUTEX_WAIT_PRIVATE, expected, nullptr) == 0;
                if (success)
                {
                    m_control.store(1, MemoryOrder::AcquireRelease);
                    m_tid = tid;
                }
                return success;
            }
        }

        u32 unlock()
        {
            auto tid = syscall(__NR_gettid);
            VERIFY(m_tid == tid);
            if (m_tid == tid) [[likely]]
            {
                bool will_unlock = m_control.load(Relaxed) == 1;
                if (will_unlock)
                {
                    m_tid = 0;
                }
                auto result = m_control.sub_fetch(1, AcquireRelease);
                syscall(SYS_futex, m_control.ptr(), FUTEX_WAKE_PRIVATE, 1, nullptr);
                return result;
            }

            __builtin_abort();
        }

        bool is_locked() const
        {
            return m_control.load(Acquire) != 0;
        }

    private:
        Atomic<u32> m_control alignas(4) { 0 };
        long m_tid { 0 };
    };

    template<MutexLike T>
    class ScopedLock
    {
    public:
        ScopedLock() = delete;
        ScopedLock& operator=(ScopedLock const&) = delete;
        ScopedLock& operator=(ScopedLock&&) = delete;

        constexpr ScopedLock(T& mutex) :
            m_mutex(mutex)
        {
            mutex.lock();
        }

        constexpr ~ScopedLock()
        {
            m_mutex.unlock();
        }

    private:
        T& m_mutex;
    };
}
using neo::Mutex;
using neo::RecursiveMutex;
using neo::ScopedLock;
