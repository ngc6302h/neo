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
#include <AsciiString.h>

int main()
{
    AsciiString a = "This is a string suited for non-unicode characters. It provides constant time indexing."_as;
    TEST_EQUAL(a.length(), 87);
    TEST_EQUAL(a.end().index() - a.begin().index(), 87);
    TEST_EQUAL(a.end().index() - a.begin().index(), 87);
    TEST_EQUAL(AsciiString(a.span().data()), a);
    TEST_EQUAL(__builtin_strcmp((char*)a, a.null_terminated_characters()), 0);
    TEST(a.contains("suited for non-unicode characters"));
    TEST_FALSE(a.contains("unicorn"));
    TEST(a.starts_with("This is a"));
    TEST_FALSE(a.starts_with("This isn't a"));
    TEST(a.ends_with("time indexing."));
    TEST_FALSE(a.ends_with("time is not real"));
    TEST_EQUAL(a[3], 's');
    TEST_EQUAL(a[86], '.');
    TEST_NOT_EQUAL(a, "other string"_as);
    TEST_EQUAL(a, AsciiString(a));
    TEST_EQUAL("abc"_as <=> "abc"_as, 0);
    TEST_EQUAL("abb"_as <=> "abc"_as, -1);
    TEST_EQUAL("abd"_as <=> "abc"_as, 1);
    TEST_EQUAL("ab"_as <=> "abc"_as, -1);
    TEST_EQUAL("ac"_as <=> "abc"_as, -1);
    TEST_EQUAL(""_as <=> "abc"_as, -1);
    return 0;
}
