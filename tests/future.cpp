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
#include <Future.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


void* lambda(void* promise_v)
{
    auto* promise = (Promise<int>*) promise_v;
    printf("Downloading...");
    promise->set_value(42);
    return nullptr;
}

Future<int> download()
{
    Promise<int> promise;
    Future<int> willbe_int = promise.get_future();
    pthread_t thread;
    pthread_create(&thread, nullptr, reinterpret_cast<void *(*)(void *)>(lambda), &promise);
    pthread_detach(thread);
    
    return willbe_int;
}

int main()
{
    auto willbe_result = download();
    while (!willbe_result.has_value())
    {
        printf("Main thread doing some other work");
        usleep(10000);
    }
    printf("The result is %d\n", willbe_result.value());
    return 0;
}