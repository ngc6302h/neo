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

namespace neo
{
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
            size_t expected = 0;
            while (!m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire))
                ;
        }

        constexpr bool try_lock()
        {
            size_t expected = 0;
            return m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire);
        }

        constexpr bool unlock()
        {
            size_t expected = 1;
            bool result = m_control.compare_exchange_strong(expected, 0, AcquireRelease, Acquire);
            return result;
        }

        constexpr bool is_locked() const
        {
            return m_control.load(Acquire) == 1;
        }

    private:
        Atomic<size_t> m_control { 0 };
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

        size_t lock()
        {
            auto tid = syscall(__NR_gettid);
            if (m_tid == tid)
            {
                return m_control.add_fetch(1, Relaxed);
            }
            else
            {
                size_t expected;
                do
                {
                    expected = 0;
                    m_control.compare_exchange_weak(expected, 1, AcquireRelease, Acquire);
                } while (expected != 0);
                m_tid = tid;
                return 1;
            }
        }

        bool try_lock()
        {
            auto tid = syscall(__NR_gettid);
            if (m_tid == tid)
            {
                m_control.add_fetch(1, Relaxed);
                return true;
            }
            else
            {
                size_t expected = 0;
                bool success = m_control.compare_exchange_strong(expected, 1, AcquireRelease, Acquire);
                if (success)
                    m_tid = tid;
                return success;
            }
        }

        void unlock()
        {
            auto tid = syscall(__NR_gettid);
            if (m_tid == tid)
            {
                bool will_unlock = m_control.load(Acquire) == 1;
                if (will_unlock)
                    m_tid = 0;
                m_control.sub_fetch(1, AcquireRelease);
            }
        }

        bool is_locked() const
        {
            return m_control.load(Acquire) != 0;
        }

    private:
        Atomic<size_t> m_control { 0 };
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
