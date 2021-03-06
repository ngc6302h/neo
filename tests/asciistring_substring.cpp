/*
    Copyright (C) 2022  César Torres (shortanemoia@protonmail.com)
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
    AsciiString a { "String A" };
    AsciiString b = a.substring(0);
    TEST_EQUAL(a, b);
    TEST_EQUAL("tring A"_as, a.substring(1));
    TEST_EQUAL(b.substring(b.length()), ""_as);
    TEST_EQUAL(b.substring(b.length(), 0), ""_as);
    TEST_EQUAL(b.substring(b.begin()), b);
    TEST_EQUAL(b.substring(b.begin(), b.length()), b);
    auto end = b.end();
    TEST_EQUAL(b.substring(--end), "A"_as);
    return 0;
}
