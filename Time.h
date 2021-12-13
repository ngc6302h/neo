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
#include "Assert.h"
#include "Checked.h"
#include <time.h>
#include "Optional.h"

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
        constexpr Time(u64 seconds, u64 nanoseconds) : m_seconds(seconds), m_nanoseconds(nanoseconds) {}
        constexpr Time() = default;
        
        constexpr Optional<Time> add(Time const& other)
        {
            Checked<u64> secs(m_seconds);
            Checked<u64> nanosecs(m_nanoseconds);
            
            secs+=other.m_seconds+(other.m_nanoseconds>=1000000000 ? 1 : 0);
            nanosecs+=other.m_nanoseconds>=1000000000 ? other.m_nanoseconds%1000000000 : other.m_nanoseconds;
            if (secs.has_overflow() || nanosecs.has_overflow())
                return {};
            return Time{secs.value(), nanosecs.value()};
        }
    
        constexpr Optional<Time> sub(Time const& other)
        {
            Checked<u64> secs(m_seconds);
            Checked<u64> nanosecs(m_nanoseconds);
        
            secs-=other.m_seconds-(other.m_nanoseconds>=1000000000 ? 1 : 0);
            nanosecs-=other.m_nanoseconds>=1000000000 ? other.m_nanoseconds%1000000000 : other.m_nanoseconds;
            if (secs.has_overflow() || nanosecs.has_overflow())
                return {};
            return Time{secs.value(), nanosecs.value()};
        }
        
        constexpr bool operator<(Time const& other)
        {
            if (m_seconds < other.m_seconds || (m_seconds == other.m_seconds && m_nanoseconds < other.m_nanoseconds))
                return true;
            return false;
        }
    
        constexpr bool operator>(Time const& other)
        {
            if (m_seconds > other.m_seconds || (m_seconds == other.m_seconds && m_nanoseconds > other.m_nanoseconds))
                return true;
            return false;
        }
        
        constexpr bool operator==(Time const& other)
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
            return Time{static_cast<u64>(time.tv_sec), static_cast<u64>(time.tv_nsec)};
        }
        
        static Time resolution()
        {
            timespec res;
            clock_getres(CLOCK_MONOTONIC_RAW, &res);
            return Time{static_cast<u64>(res.tv_sec), static_cast<u64>(res.tv_nsec)};
        }
    };
    
    struct Clock
    {
        static Time now()
        {
            timespec time;
            clock_gettime(CLOCK_REALTIME, &time);
            return Time{static_cast<u64>(time.tv_sec), static_cast<u64>(time.tv_nsec)};
        }
    
        static Time resolution()
        {
            timespec res;
            clock_getres(CLOCK_REALTIME, &res);
            return Time{static_cast<u64>(res.tv_sec), static_cast<u64>(res.tv_nsec)};
        }
    };
    
}