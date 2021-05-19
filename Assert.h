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

extern "C"
{
    [[noreturn]] extern void
    __assert_fail(const char* __assertion, const char* __file, unsigned int __line, const char* __function) noexcept(true);
}

#ifdef DEBUG
    #define VERIFY(expr) (static_cast<bool>(expr) ? void(0) : __assert_fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__))
    #define VERIFY_NOT_REACHED() VERIFY(false)
#else
    #define VERIFY(expr)
    #define VERIFY_NOT_REACHED() /*abort()*/ __builtin_trap()
#endif