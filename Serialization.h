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

#pragma once
#include "Types.h"
#include "Util.h"
#include "TypeExtras.h"
#include "Concepts.h"
#include "Vector.h"
#include "Optional.h"

namespace neo
{
    enum class SerializationEndianness
    {
        Little,
        Big
    };

    static constexpr auto HostEndianness = LittleEndian ? SerializationEndianness::Little : SerializationEndianness::Big;

    template<typename T>
    struct BinaryFormatter
    {
        static constexpr size_t serialize_into(T const& object, Span<u8> buffer);
        static constexpr Optional<T> deserialize_from(Span<u8> const& buffer);
    };

    template<Scalar T>
    struct BinaryFormatter<T>
    {
        template<SerializationEndianness Endianness = HostEndianness>
        static constexpr size_t serialize_into(T const& object, Span<u8> buffer)
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

        template<SerializationEndianness Endianness = SerializationEndianness::Little>
        static constexpr Optional<T> deserialize_from(Span<u8> const& buffer)
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

    template<>
    struct BinaryFormatter<String>
    {
        template<SerializationEndianness Endianness = HostEndianness>
        static constexpr size_t serialize_to(String const& str, Span<u8> buffer)
        {
            VERIFY(buffer.size() >= str.byte_size() + sizeof(size_t));
            BinaryFormatter<size_t>::serialize_into<Endianness>(str.byte_size(), buffer);
            Copy(str.byte_size(), (u8*)str.data(), buffer.slice(sizeof(size_t)).data());
            return str.byte_size() + sizeof(size_t);
        }

        template<SerializationEndianness Endianness = HostEndianness>
        static constexpr Optional<String> deserialize_from(Span<u8> const& buffer)
        {
            VERIFY(buffer.size() > sizeof(u64));
            u64 string_length = BinaryFormatter<u64>::deserialize_from<Endianness>(buffer);
            auto string_bytes = buffer.slice(sizeof(u64), string_length);
            String deserialized_string((const char*)string_bytes.data(), string_bytes.size());
            return deserialized_string;
        }
    };
}
using neo::BinaryFormatter;
