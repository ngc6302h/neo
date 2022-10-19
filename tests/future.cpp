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

#include "Test.h"
#include "ScopeExit.h"
#include <Future.h>
#include <Thread.h>
#include <stdio.h>

void lambda(Promise<int>& promise)
{
    printf("Downloading...");
    promise.set_value(42);
    printf("Downloaded! ");
}

Future<int> download()
{
    Promise<int> promise;
    Future<int> willbe_int = promise.get_future();
    auto maybe_thread = Thread::create([p = move(promise)]() mutable
        {
        printf("Downloading...");
        p.set_value(42);
        printf("Downloaded! "); });

    VERIFY(maybe_thread.has_value());
    return willbe_int;
}

int main()
{
    setbuf(stdout, nullptr);

    auto willbe_result = download();
    while (!willbe_result.has_value())
    {
        printf("Main thread doing some other work\n");
    }
    printf("The result is %d\n", willbe_result.value());
    TEST_EQUAL(willbe_result.value(), 42);
    return 0;
}
