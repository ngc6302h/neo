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
#include <AsciiStringView.h>

int main()
{
    constexpr AsciiStringView a = "This is a constexpr string";
    constexpr AsciiStringView b(a);
    TEST_EQUAL(__builtin_strcmp("This is a constexpr string", a.non_null_terminated_buffer()), 0);
    constexpr AsciiStringView c("This is another constexpr string");
    TEST_EQUAL(__builtin_strcmp("This is another constexpr string", c.non_null_terminated_buffer()), 0);
    constexpr auto d = "This is yet another way to construct another constexpr string"_asv;
    TEST_EQUAL(__builtin_strcmp("This is yet another way to construct another constexpr string", d.non_null_terminated_buffer()), 0);
    constexpr auto e = d;
    TEST_EQUAL(__builtin_strcmp(d.non_null_terminated_buffer(), e.non_null_terminated_buffer()), 0);
    constexpr auto f = move(d);
    TEST_EQUAL(__builtin_strcmp("This is yet another way to construct another constexpr string", f.non_null_terminated_buffer()), 0);
    return 0;
}
