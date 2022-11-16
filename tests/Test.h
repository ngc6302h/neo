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
#if DEBUG_ASSERTS == 0
    #undef DEBUG_ASSERTS
    #undef VERBOSE_ASSERTS
    #define DEBUG_ASSERTS 1
    #define VERBOSE_ASSERTS 1
#endif
#include <Assert.h>

#define TEST(expr) VERIFY(expr)
#define TEST_FALSE(expr) VERIFY(!(expr))
#define TEST_EQUAL(expr1, expr2) VERIFY(expr1 == expr2)
#define TEST_NOT_EQUAL(expr1, expr2) VERIFY(expr1 != expr2)
#define TEST_UNREACHABLE() VERIFY_NOT_REACHED()

struct LifetimeLogger
{
    LifetimeLogger(char const* msg = "") :
        msg(msg)
    {
        __builtin_printf("Object \"%s\" constructed\n", msg);
    }

    ~LifetimeLogger()
    {
        __builtin_printf("Object \"%s\" destroyed\n", msg);
    }

    LifetimeLogger(LifetimeLogger const& other)
    {
        msg = other.msg;
        __builtin_printf("Object \"%s\" copied\n", msg);
    }

    LifetimeLogger(LifetimeLogger&& other)
    {
        msg = other.msg;
        other.msg = "(moved away)";
        __builtin_printf("Object \"%s\" moved\n", msg);
    }

    LifetimeLogger& operator=(LifetimeLogger const&)
    {
        __builtin_printf("Object \"%s\" copy assigned\n", msg);
        return *this;
    }

    LifetimeLogger& operator=(LifetimeLogger&&)
    {
        __builtin_printf("Object \"%s\" move assigned\n", msg);
        return *this;
    }

    char const* msg;
};
