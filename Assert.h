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
#if not defined(_MSC_VER) and not defined(__MINGW32__)
#include <execinfo.h>
extern "C"
{
    [[noreturn]] extern void __assert_fail(const char* __assertion, const char* __file, unsigned int __line, const char* __function) noexcept(true);
}
#else
#define __assert_fail
#endif

static char __backtrace_buffer[sizeof(void*) * 256];
[[maybe_unused]] [[noreturn]] [[gnu::noinline]] static void print_backtrace_and_fail(const char* __assertion, const char* __file, unsigned int __line, const char* __function)
{
#if not defined(_MSC_VER) and not defined(__MINGW32__)
    __builtin_printf("Backtrace for failed thread:\n");
    auto num_addresses = backtrace(reinterpret_cast<void**>(__backtrace_buffer), 256);
    backtrace_symbols_fd(reinterpret_cast<void* const*>(__backtrace_buffer), num_addresses, 1);
    __assert_fail(__assertion, __file, __line, __function);
#endif
	__builtin_printf("Assertion '%s' failed!\nAt file %s, line %i\nFunction:\n%s", __assertion, __file, __line, __function);
	while(true);
}

#if VERBOSE_ASSERTS == 1
    #define __NEO_ASSERT_FAILED print_backtrace_and_fail
#else
    #define __NEO_ASSERT_FAILED __assert_fail
#endif

#if DEBUG_ASSERTS == 1
    #define VERIFY(expr)                                                         \
        if (!(expr)) [[unlikely]]                                                \
        {                                                                        \
            __NEO_ASSERT_FAILED(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
        }
    #define VERIFY_NOT_REACHED() __NEO_ASSERT_FAILED("Reached unreachable code!", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#else
    #define VERIFY(expr)
    #define VERIFY_NOT_REACHED() print_backtrace_and_fail("Reached unreachable code!", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#define ENSURE(expr)                                                         \
    if (!(expr)) [[unlikely]]                                                \
    {                                                                        \
        __NEO_ASSERT_FAILED(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
    }
