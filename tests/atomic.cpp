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

#include "Test.h"
#include <pthread.h>
#include <Atomic.h>

Atomic<u32> var {};

void* increment(void*)
{
    while (true)
    {
        u32 value = var.load(neo::Acquire);
        if (value == (u32)0x02FFFFFF)
            return reinterpret_cast<void*>(value);
        var.compare_exchange_strong(value, value + 1, neo::AcquireRelease, neo::Acquire);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, nullptr, &increment, nullptr);
    pthread_create(&tid, nullptr, &increment, nullptr);
    pthread_create(&tid, nullptr, &increment, nullptr);
    pthread_create(&tid, nullptr, &increment, nullptr);

    while (true)
    {
        u32 v = var.load(MemoryOrder::Acquire);
        if (v == (u32)0x02FFFFFF)
            break;
    };
    VERIFY(var.load(MemoryOrder::Relaxed) == 0x02FFFFFF);
}
