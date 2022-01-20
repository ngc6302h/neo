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
#define DEBUG_ASSERTS 1
#define VERBOSE_ASSERTS 1
#include <Assert.h>

#define TEST(expr) VERIFY(expr)
#define TEST_FALSE(expr) VERIFY(!(expr))
#define TEST_EQUAL(expr1, expr2) VERIFY(expr1 == expr2)
#define TEST_NOT_EQUAL(expr1, expr2) VERIFY(expr1 != expr2)
#define TEST_UNREACHABLE() VERIFY_NOT_REACHED()

struct LifetimeLogger
{
    LifetimeLogger()
    {
        __builtin_printf("Object constructed\n");
    }
    
    ~LifetimeLogger()
    {
        __builtin_printf("Object destroyed\n");
    }
    
    LifetimeLogger(LifetimeLogger const&)
    {
        __builtin_printf("Object copied\n");
    }
    
    LifetimeLogger(LifetimeLogger&&)
    {
        __builtin_printf("Object moved\n");
    }
    
    LifetimeLogger& operator=(LifetimeLogger const&)
    {
        __builtin_printf("Object copy assigned\n");
        return *this;
    }
    
    LifetimeLogger& operator=(LifetimeLogger&&)
    {
        __builtin_printf("Object move assigned\n");
        return *this;
    }
};