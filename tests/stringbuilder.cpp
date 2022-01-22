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
#include <StringBuilder.h>

int main()
{
    return 0;
    StringBuilder sb;
    sb.append("   __This is__ a strin__g__     ");
    sb.replace("_", ".");
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "   ..This is.. a strin..g..     ") == 0);
    sb.replace("   ..This is.. a strin..g..     ", "   ..This is.. a strin..g..     ");
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "   ..This is.. a strin..g..     ") == 0);
    sb.remove("..");
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "   This is a string     ") == 0);
    sb.trim_whitespace((neo::TrimMode)(neo::TrimMode::Start | neo::TrimMode::End));
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "This is a string") == 0);
    sb.append("!!!");
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "This is a string!!!") == 0);
    sb.remove("This is a string!!!");
    TEST(__builtin_strcmp(sb.to_string().null_terminated_characters(), "") == 0);
    return 0;
}
