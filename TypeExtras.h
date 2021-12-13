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

#include "String.h"
#include "TypeTraits.h"
#include "Preprocessor.h"

namespace neo
{
    template<typename T>
    consteval StringView get_type_name()
    {
        constexpr StringView function_signature = __PRETTY_FUNCTION__;
        constexpr auto type_name = function_signature.substring(57);
        constexpr auto ready = type_name.substring(0, type_name.length() - 1);
        return ready;
    }

    template<typename T>
    constinit StringView nameof = get_type_name<T>();

    template<typename T>
    constexpr StringView dynamic_nameof(T const& v);

#define STRINGIFIABLE_ENUM(enum_name, ...)                             \
    enum class enum_name                                               \
    {                                                                  \
        __VA_ARGS__                                                    \
    };                                                                 \
    template<>                                                         \
    constexpr StringView dynamic_nameof<enum_name>(enum_name const& v) \
    {                                                                  \
        using enum enum_name;                                          \
        switch (v)                                                     \
        {                                                              \
            __FOR_EACH(__ENUM_CASE, __VA_ARGS__)                       \
        default:                                                       \
            return "undefined";                                        \
        }                                                              \
    }

}
using neo::nameof;
