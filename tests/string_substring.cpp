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
#include <String.h>

int main()
{
    String a { "String A" };
    String b = a.substring(0);
    TEST_EQUAL(a, b);
    TEST_EQUAL(String { "tring A" }, a.substring(1));
    TEST_EQUAL(b.substring(b.length()), ""_s);
    TEST_EQUAL(b.substring(b.length(), 0), ""_s);
    TEST_EQUAL(b.substring(b.cbegin()), b);
    TEST_EQUAL(b.substring(b.cbegin(), b.length()), b);
    auto end = b.cend();
    TEST_EQUAL(b.substring(--end), ""_s);
    return 0;
}