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

#pragma once
#include <Assert.h>

#define TEST(expr) VERIFY(expr)
#define TEST_FALSE(expr) VERIFY(!(expr))
#define TEST_EQUAL(expr1, expr2) VERIFY(expr1 == expr2)
#define TEST_NOT_EQUAL(expr1, expr2) VERIFY(expr1 != expr2)
#define TEST_UNREACHABLE() VERIFY_NOT_REACHED()
