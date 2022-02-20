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
#include "Types.h"

namespace neo
{
    template<template<typename T, auto...> typename TContainer, typename T>
    struct IContainer
    {
        using type = T;
        template<typename U>
        using container_type = TContainer<U>;
    };

    template<size_t Size>
    struct IFixedSizeContainer
    {
        static constexpr size_t size() { return Size; }
    };
}
using neo::IContainer;
using neo::IFixedSizeContainer;
