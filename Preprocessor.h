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

//The following method to implement a FOR_EACH macro is extracted from https://www.scs.stanford.edu/~dm/blog/va-opt.html
#define __PARENS ()
#define __EXPAND(...) __EXPAND4(__EXPAND4(__EXPAND4(__EXPAND4(__VA_ARGS__))))
#define __EXPAND4(...) __EXPAND3(__EXPAND3(__EXPAND3(__EXPAND3(__VA_ARGS__))))
#define __EXPAND3(...) __EXPAND2(__EXPAND2(__EXPAND2(__EXPAND2(__VA_ARGS__))))
#define __EXPAND2(...) __EXPAND1(__EXPAND1(__EXPAND1(__EXPAND1(__VA_ARGS__))))
#define __EXPAND1(...) __VA_ARGS__
#define __FOR_EACH(macro, ...)                                    \
  __VA_OPT__(__EXPAND(__FOR_EACH_HELPER(macro, __VA_ARGS__)))
#define __FOR_EACH_HELPER(macro, a1, ...)                         \
  macro(a1)                                                       \
  __VA_OPT__(__FOR_EACH_AGAIN __PARENS (macro, __VA_ARGS__))
#define __FOR_EACH_AGAIN() __FOR_EACH_HELPER

#define STRINGIFY(x) #x

#define __ENUM_CASE(name) case name: return #name;

