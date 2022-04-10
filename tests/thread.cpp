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

void entry(String const& msg)
{
    puts("Hello world 👋 from another thread");
    printf("%s\n", msg.null_terminated_characters());
    sleep(1);
    char name[32];
    auto result = pthread_getname_np(pthread_self(), name, 32);
    if (result != 0)
    {
        printf("%d\n", result);
    }
    printf("My thread name is: %s\n", name);
}

int main()
{
    auto* s = new String("I am just a poor string from another thread");
    auto maybe_thread = Thread::create([&]()
        { entry(*s); });
    if (maybe_thread.has_error())
        return -1;
    auto thread = move(maybe_thread.result());
    __builtin_printf("Hello world 👋\n");
    thread->set_name("second thread!");
    sleep(2);
}
