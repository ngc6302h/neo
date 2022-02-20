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
#include "execinfo.h"

extern "C"
{
    [[noreturn]] extern void __assert_fail(const char* __assertion, const char* __file, unsigned int __line, const char* __function) noexcept(true);
}

static char __backtrace_buffer[sizeof(void*) * 256];
[[maybe_unused]] [[noreturn]] static void print_backtrace_and_fail(const char* __assertion, const char* __file, unsigned int __line, const char* __function)
{
    __builtin_printf("Backtrace for failed thread:\n");
    auto num_addresses = backtrace(reinterpret_cast<void**>(__backtrace_buffer), 256);
    char** resolved_symbols = backtrace_symbols(reinterpret_cast<void* const*>(__backtrace_buffer), num_addresses);
    for (int i = 0; i < num_addresses; ++i)
        __builtin_printf("%s\n", resolved_symbols[i]);
    __builtin_free(resolved_symbols);
    __assert_fail(__assertion, __file, __line, __function);
}

#if VERBOSE_ASSERTS == 1
    #define __ASSERT_FAILED print_backtrace_and_fail
#else
    #define __ASSERT_FAILED __assert_fail
#endif

#if DEBUG_ASSERTS == 1
    #define VERIFY(expr)                                                     \
        if (!(expr)) [[unlikely]]                                            \
        {                                                                    \
            __ASSERT_FAILED(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        }
    #define VERIFY_NOT_REACHED() __ASSERT_FAILED("Reached unreachable code!", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#else
    #define VERIFY(expr)
    #define VERIFY_NOT_REACHED() print_backtrace_and_fail("Reached unreachable code!", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
