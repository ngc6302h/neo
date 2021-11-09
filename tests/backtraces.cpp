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
#include <unistd.h>
#include <wait.h>

__attribute__((noinline)) int oh_no()
{
    VERIFY_NOT_REACHED();
}

namespace neo
{
    namespace bar
    {
        __attribute__((noinline)) int j()
        {
            return oh_no();
        }
    }
    
    namespace foo
    {
        __attribute__((noinline)) long i()
        {
            return bar::j();
        }
    }
    
    __attribute__((noinline)) int g()
    {
        return foo::i();
    }
    
    namespace test
    {
        __attribute__((noinline)) int f()
        {
            return g();
        }
    }
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        auto result = fork();
        if (result == 0)
        {
            //new process
            execl(argv[0], argv[0], "dummy", nullptr);
            return -1;
        }
        else if (result > 0)
        {
            int status {};
            waitpid(result, &status, 0);
            TEST_FALSE(WIFEXITED(status));
        }
    }
    else
    {
        neo::test::f();
    }
}



