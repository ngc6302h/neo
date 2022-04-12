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
#include <AsciiStringView.h>

int main()
{
    constexpr auto a = "This program is free software"_asv;
    TEST_EQUAL(__builtin_strcmp(a.substring_view(0).non_null_terminated_buffer(), "This program is free software"), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(29).non_null_terminated_buffer(), ""), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(0, 29).non_null_terminated_buffer(), "This program is free software"), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(29, 0).non_null_terminated_buffer(), ""), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(21, 8).non_null_terminated_buffer(), "software"), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(a.begin()).non_null_terminated_buffer(), "This program is free software"), 0);
    TEST_EQUAL(__builtin_strcmp(a.substring_view(a.begin(), 28).non_null_terminated_buffer(), "This program is free software"), 0);
    auto it = a.end();
    TEST_EQUAL(__builtin_strncmp(a.substring_view(--it, 0).non_null_terminated_buffer(), "e", 0), 0);
    return 0;
}
