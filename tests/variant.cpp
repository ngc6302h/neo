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
#include <Variant.h>
#include <String.h>
#include <TypeExtras.h>

int main()
{
    Variant<int, String, LifetimeLogger, bool> variant { LifetimeLogger() };
    __builtin_printf("%s\n", String(nameof<decltype(variant)>).null_terminated_characters());
    TEST_EQUAL(nameof<decltype(variant)>, "Variant<int, String, LifetimeLogger, bool>");
    variant = "Hello world"_s;
    TEST(variant.check_type_active<String>());
    TEST_FALSE(variant.check_type_active<int>());
}
