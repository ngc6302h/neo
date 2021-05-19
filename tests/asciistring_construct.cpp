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
#include <AsciiString.h>

int main()
{
    AsciiString a { "String A" };
    TEST_EQUAL( __builtin_strcmp((char*)a, "String A"), 0);
    AsciiString b = "String B"_as;
    TEST_EQUAL( __builtin_strcmp((char*)b, "String B"), 0);
    TEST_EQUAL( __builtin_strcmp((char*)AsciiString("String C"), "String C"), 0);
    TEST_EQUAL( __builtin_strcmp((char*)AsciiString(""), ""), 0);
    [[maybe_unused]] AsciiString c = move (b);
    TEST(!c.is_empty());
    TEST(b.is_empty());
    TEST_EQUAL(c.length(), AsciiString("String B").length());
    return 0;
}