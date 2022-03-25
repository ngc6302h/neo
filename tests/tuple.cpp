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
#include <Tuple.h>

int main()
{
    int _a = 2;
    Tuple<ReferenceWrapper<int>, bool, size_t> tup { _a, true, (size_t)5 };
    TEST_EQUAL(tup.get<ReferenceWrapper<int>>(), 2);
    TEST_EQUAL(tup.get<bool>(), true);
    TEST_EQUAL(tup.get<size_t>(), 5);
    TEST_EQUAL(tup.get<0>(), 2);
    TEST_EQUAL(tup.get<1>(), true);
    TEST_EQUAL(tup.get<2>(), 5);
    [[maybe_unused]] auto& [a, b, c] = tup;
    a = 9;
    TEST_EQUAL(a, _a);
    [[maybe_unused]] auto const& [aref, bref, cref] = tup;
    TEST_EQUAL(a, 9);
    TEST_EQUAL(b, true);
    TEST_EQUAL(c, 5);
    return 0;
}
