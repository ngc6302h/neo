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
#include "TypeExtras.h"
#include "Concepts.h"
#include "Vector.h"
#include "Optional.h"

namespace neo
{
    STRINGIFIABLE_ENUM(SerializationEndianness, Little, Big)

    template<typename T>
    struct BinaryFormatter
    {
        constexpr size_t serialize_into(T const& object, Span<u8>& buffer);
        constexpr Optional<T> deserialize_from(Span<u8> const& buffer);
    };

    template<Scalar T>
    struct BinaryFormatter<T>
    {
        template<SerializationEndianness Endianness>
        constexpr size_t serialize_into(T const& object, Span<u8>& buffer)
        {
            VERIFY(buffer.size() >= sizeof(T));
            if constexpr ((LittleEndian && Endianness == SerializationEndianness::Little) || (BigEndian && Endianness == SerializationEndianness::Big) || (sizeof(T) == 1))
            {
                reinterpret_cast<T*>(buffer.data()) = object;
            }
            else
            {
                if constexpr (sizeof(T) == 2)
                {
                    reinterpret_cast<T*>(buffer.data()) = __builtin_bswap16(object);
                }
                else if constexpr (sizeof(T) == 4)
                {
                    reinterpret_cast<T*>(buffer.data()) = __builtin_bswap32(object);
                }
                else if constexpr (sizeof(T) == 8)
                {
                    reinterpret_cast<T*>(buffer.data()) = __builtin_bswap64(object);
                }
                else if constexpr (sizeof(T) == 16)
                {
                    reinterpret_cast<T*>(buffer.data()) = __builtin_bswap128(object);
                }
            }
            return sizeof(T);
        }

        template<SerializationEndianness Endianness>
        constexpr Optional<T> deserialize_from(Span<u8> const& buffer)
        {
            VERIFY(buffer.size() >= sizeof(T));
            if constexpr ((LittleEndian && Endianness == SerializationEndianness::Little) || (BigEndian && Endianness == SerializationEndianness::Big) || (sizeof(T) == 1))
            {
                return *reinterpret_cast<T*>(buffer.data());
            }
            else
            {
                if constexpr (sizeof(T) == 2)
                {
                    return __builtin_bswap16(*reinterpret_cast<T*>(buffer.data()));
                }
                else if constexpr (sizeof(T) == 4)
                {
                    return __builtin_bswap32(*reinterpret_cast<T*>(buffer.data()));
                }
                else if constexpr (sizeof(T) == 8)
                {
                    return __builtin_bswap64(*reinterpret_cast<T*>(buffer.data()));
                }
                else if constexpr (sizeof(T) == 16)
                {
                    return __builtin_bswap128(*reinterpret_cast<T*>(buffer.data()));
                }
            }
            VERIFY_NOT_REACHED();
        }
    };
}
using neo::BinaryFormatter;
