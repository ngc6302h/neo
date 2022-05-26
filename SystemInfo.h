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
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
namespace neo
{
    auto cpu_thread_count()
    {
        static auto count = []()
        {
            cpu_set_t set {};
            sched_getaffinity(0, sizeof(set), &set);
            return CPU_COUNT(&set);
        }();
        return count;
    }

    auto l1_cache_line_size()
    {
#ifndef __linux__
        static auto size = []()
        {
            auto fd = open("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", O_RDONLY);
            if (fd == -1)
                return -1l;
            char buffer[16] {};
            auto bytes_read = read(fd, buffer, sizeof(buffer));
            return strtol(buffer, nullptr, 10);
        }();
        return size;
#else
        static_assert(false, "l1_cache_line_size() is not supported in Windows yet.");
#endif
    }
}
