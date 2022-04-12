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
#include "Assert.h"
#include "Checked.h"
#include "Optional.h"
#include <time.h>

namespace neo
{
    enum class ClockType
    {
        Realtime,
        Monotonic
    };

    class Time
    {
    public:
        constexpr Time(u64 seconds, u64 nanoseconds) :
            m_seconds(seconds), m_nanoseconds(nanoseconds) { }
        constexpr Time() = default;

        constexpr Optional<Time> add(Time const& other) const
        {
            Checked<u64> secs(m_seconds);
            Checked<u64> nanosecs(m_nanoseconds);

            secs += other.m_seconds + (other.m_nanoseconds >= 1000000000 ? 1 : 0);
            nanosecs += other.m_nanoseconds >= 1000000000 ? other.m_nanoseconds % 1000000000 : other.m_nanoseconds;
            if (secs.has_overflow() || nanosecs.has_overflow())
                return {};
            return Time { secs.value(), nanosecs.value() };
        }

        constexpr Optional<Time> sub(Time const& other) const
        {
            Checked<u64> secs(m_seconds);
            Checked<u64> nanosecs(m_nanoseconds);

            secs -= other.m_seconds - (other.m_nanoseconds >= 1000000000 ? 1 : 0);
            nanosecs -= other.m_nanoseconds >= 1000000000 ? other.m_nanoseconds % 1000000000 : other.m_nanoseconds;
            if (secs.has_overflow() || nanosecs.has_overflow())
                return {};
            return Time { secs.value(), nanosecs.value() };
        }

        constexpr bool operator<(Time const& other) const
        {
            if (m_seconds < other.m_seconds || (m_seconds == other.m_seconds && m_nanoseconds < other.m_nanoseconds))
                return true;
            return false;
        }

        constexpr bool operator>(Time const& other) const
        {
            if (m_seconds > other.m_seconds || (m_seconds == other.m_seconds && m_nanoseconds > other.m_nanoseconds))
                return true;
            return false;
        }

        constexpr bool operator==(Time const& other) const
        {
            return m_seconds == other.m_seconds && m_nanoseconds == other.m_nanoseconds;
        }

        constexpr Time& operator+=(Time const& other)
        {
            this->m_seconds += other.m_seconds;
            this->m_nanoseconds += other.m_nanoseconds;

            return *this;
        }

        constexpr Time& operator-=(Time const& other)
        {
            this->m_seconds -= other.m_seconds;
            this->m_nanoseconds -= other.m_nanoseconds;

            return *this;
        }

    private:
        u64 m_seconds {};
        u64 m_nanoseconds {};
    };

    struct Timer
    {
        static Time now()
        {
            timespec time;
            clock_gettime(CLOCK_MONOTONIC_RAW, &time);
            return Time { static_cast<u64>(time.tv_sec), static_cast<u64>(time.tv_nsec) };
        }

        static Time resolution()
        {
            timespec res;
            clock_getres(CLOCK_MONOTONIC_RAW, &res);
            return Time { static_cast<u64>(res.tv_sec), static_cast<u64>(res.tv_nsec) };
        }
    };

    struct Clock
    {
        static Time now()
        {
            timespec time;
            clock_gettime(CLOCK_REALTIME, &time);
            return Time { static_cast<u64>(time.tv_sec), static_cast<u64>(time.tv_nsec) };
        }

        static Time resolution()
        {
            timespec res;
            clock_getres(CLOCK_REALTIME, &res);
            return Time { static_cast<u64>(res.tv_sec), static_cast<u64>(res.tv_nsec) };
        }
    };

    class ScopedTimer
    {
        explicit ScopedTimer(Time& store) :
            m_start_time(Timer::now()), m_store(store)
        {
        }

        ~ScopedTimer()
        {
            m_store = Timer::now().sub(m_start_time).release_value();
        }

    private:
        Time m_start_time;
        Time& m_store;
    };

}
using neo::ScopedTimer;
using neo::Time;
using neo::Timer;
