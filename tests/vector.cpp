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

#include <Vector.h>
#include "Test.h"

int main()
{
    const auto print = [](auto const& vec)
    { for (auto const& v : vec) __builtin_printf("%d ", v); __builtin_printf("\n"); };

    Vector<int> reference { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    Vector<int> pure_heap { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    TEST_EQUAL(pure_heap, reference);
    print(pure_heap);
    Vector<int, 10> pure_inline = pure_heap;
    TEST_EQUAL(pure_inline, reference);
    print(pure_inline);
    Vector<int, 5> mixed_from_pure_heap = pure_heap;
    TEST_EQUAL(mixed_from_pure_heap, reference);
    print(mixed_from_pure_heap);
    Vector<int, 5> mixed_from_pure_inline = pure_inline;
    TEST_EQUAL(mixed_from_pure_inline, reference);
    print(mixed_from_pure_inline);
    pure_heap = mixed_from_pure_inline;
    TEST_EQUAL(pure_heap, reference);
    print(pure_heap);
    pure_inline = mixed_from_pure_heap;
    TEST_EQUAL(pure_inline, reference);
    print(pure_inline);
    return 0;
}
