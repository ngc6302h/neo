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
    constexpr auto a = "This program is free software"_asv;
    constexpr auto b = a;
    TEST(a == b);
    TEST_FALSE(a != b);
    TEST_EQUAL(a <=> b, 0);
    __builtin_printf("%d\n", "This program is free sof"_asv <=> "This program is free software"_asv);
    TEST_EQUAL("This program is free sof"_asv <=> "This program is free software"_asv, -1);
    TEST_EQUAL("This program is free software"_asv <=> "This program is free sof"_asv, 1);
    TEST_FALSE(a.is_empty());
    TEST(""_asv.is_empty());
    TEST_EQUAL(a.length(), 29);
    auto it = a.begin();
    TEST_EQUAL(*it, 'T');
    it = a.end();
    TEST_EQUAL(*--it, 'e');
    it = a.begin();
    TEST_EQUAL(*it, 'T');
    it = a.end();
    TEST_EQUAL(*--it, 'e');
    TEST_EQUAL(__builtin_strcmp(a.span().data(), "This program is free software"), 0);
    TEST_EQUAL(a.span().size(), 29);
    TEST_EQUAL(__builtin_strcmp(a.non_null_terminated_buffer(), "This program is free software"), 0);
    TEST_EQUAL(a[0], 'T');
    TEST_EQUAL(a[28], 'e');
    TEST(a.contains("free software"));
    TEST(a.ends_with("free software"));
    TEST(a.starts_with("This program is"));
    return 0;
}