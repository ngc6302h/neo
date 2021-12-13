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
    TEST(String().is_empty());
    TEST_FALSE(String("This is a string").is_empty());
    TEST_EQUAL(__builtin_strcmp("Stri", (char const*)String("String A", 4)), 0);
    TEST_EQUAL(__builtin_strcmp(String("This is a string"), "This is a string"), 0);
    TEST_FALSE("This is a string"_s.is_empty());
    TEST(""_s.is_empty());
    TEST_EQUAL(__builtin_strlen(String("Another string"_s)), 14);
    String a { "This is a string" };
    TEST_EQUAL(__builtin_strlen(String("")), 0);
    String b = move(a);
    TEST_EQUAL(__builtin_strcmp(b, "This is a string"), 0);
    String c = b;
    TEST_EQUAL(__builtin_strcmp(b, c), 0);
    TEST_EQUAL(__builtin_strcmp(String(""), ""), 0);
    return 0;
}