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
#include "NumericLimits.h"
#include <syscall.h>
#include <linux/futex.h>
#include <unistd.h>

namespace neo
{
    class Barrier
    {
    public:
        explicit Barrier(u32 expected) :
            m_control(expected), m_expected(expected)
        {
        }

        void arrive_and_wait()
        {
            auto count = m_control.sub_fetch(1, AcquireRelease);
            if (count == 0)
            {

                m_control.store(m_expected, Relaxed);
                syscall(SYS_futex, m_control.ptr(), FUTEX_WAKE_PRIVATE, NumericLimits<int>::max());
            }
            else
                wait();
        }

        void wait()
        {
            auto count = m_control.load(MemoryOrder::Acquire);
            while (count != 0)
            {
                auto result = syscall(SYS_futex, m_control.ptr(), FUTEX_WAIT_PRIVATE, count, nullptr);
                if (result == 0)
                    break;
                count = m_control.load(MemoryOrder::Acquire);
            }
        }

        void arrive()
        {
            m_control.sub_fetch(1, AcquireRelease);
        }

        void arrive_and_drop()
        {
            m_expected.sub_fetch(1, AcquireRelease);
            auto count = m_control.sub_fetch(1, AcquireRelease);
            if (count == 0)
            {
                m_control.store(m_expected, Release);
                syscall(SYS_futex, m_control.ptr(), FUTEX_WAKE_PRIVATE, (int)NumericLimits<int>::max());
            }
            else
                wait();
        }

    private:
        Atomic<u32> m_control;
        Atomic<u32> m_expected;
    };
}
using neo::Barrier;
