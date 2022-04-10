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
#include <Thread.h>
#include <stdio.h>
#include <Barrier.h>

void entry(String const& msg, Barrier& barrier)
{
    puts("Hello world 👋 from another thread");
    printf("%s\n", msg.null_terminated_characters());
    char name[32];
    barrier.arrive_and_wait();
    auto result = pthread_getname_np(pthread_self(), name, 32);
    if (result != 0)
    {
        printf("%d\n", result);
    }
    printf("My thread name is: %s\n", name);
    sleep(1);
    barrier.arrive_and_drop();
}

int main()
{
    auto* s = new String("I am just a poor string from another thread");
    Barrier barrier(2);
    auto maybe_thread = Thread::create([&]()
        { entry(*s, barrier); });
    if (maybe_thread.has_error())
        return -1;
    auto thread = move(maybe_thread.result());
    printf("Hello world 👋\n");
    thread->set_name("second thread!");
    barrier.arrive_and_wait();
    barrier.arrive_and_drop();
}
