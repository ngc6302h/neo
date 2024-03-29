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
#include <String.h>

int main()
{
    String a = "This is a string suited for unicode characters."_s;
    TEST_EQUAL(a.length(), 47);
    TEST_EQUAL(a.byte_size(), 47);
    TEST_EQUAL(a.end().ptr() - a.begin().ptr(), 47);
    TEST_EQUAL(a.end().ptr() - a.begin().ptr(), 47);
    TEST_EQUAL(String(a.span().data()), a);
    TEST_EQUAL(__builtin_strcmp((char const*)a, a.null_terminated_characters()), 0);
    TEST(a.contains("suited for unicode characters"_sv));
    TEST_FALSE(a.contains("unicorn"_sv));
    TEST(a.starts_with("This is a"_sv));
    TEST_FALSE(a.starts_with("This isn't a"_sv));
    TEST(a.ends_with("characters."));
    TEST_FALSE(a.ends_with("time is not real"_sv));
    TEST_EQUAL(a.null_terminated_characters()[3], 's');
    TEST_EQUAL(a.null_terminated_characters()[46], '.');
    TEST_NOT_EQUAL(a, "other string"_sv);
    TEST_EQUAL(a, String(a));
    String b = "こんにちわ"_s;
    __builtin_printf("%s\n", b.null_terminated_characters());
    __builtin_printf("%zu\n", b.byte_size());
    __builtin_printf("%zu\n", b.length());
    TEST_EQUAL(b.length(), 5);
    TEST_EQUAL(b.byte_size(), 15);
    [[maybe_unused]] String _ = move(b);
    TEST(b.is_empty());
    TEST_EQUAL("abc"_s <=> "abc"_s, 0);
    TEST_EQUAL("abb"_s <=> "abc"_s, -1);
    TEST_EQUAL("abd"_s <=> "abc"_s, 1);
    TEST_EQUAL("ab"_s <=> "abc"_s, -1);
    TEST_EQUAL("ac"_s <=> "abc"_s, -1);
    TEST_EQUAL(""_s <=> "abc"_s, -1);
    return 0;
}
