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
#include <ResultOrError.h>

ResultOrError<int, bool> generate(bool success)
{
    if (success)
        return 42;
    else
        return false;
}

int main()
{
    auto test1 = generate(false);
    TEST(test1.has_error());
    TEST_EQUAL(test1.error(), false);
    TEST_EQUAL(test1.result_or(42), 42);
    auto test2 = generate(true);
    TEST(test2.has_value());
    TEST_EQUAL(test2.result(), 42);
    return 0;
}
