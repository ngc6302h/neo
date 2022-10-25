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
#include "Stream.h"
#include "Array.h"
#include "Serialization.h"
#include "Optional.h"

namespace neo
{
    class BinaryReader
    {
    public:
        explicit BinaryReader(InputStream& stream) :
            m_base_stream(stream)
        {
        }

        template<typename T, size_t VBinarySize = sizeof(T)>
        Optional<T> read()
        {
            Array<u8, sizeof(T)> buf;
            if (m_base_stream.read(buf.span()) != VBinarySize)
                return {};
            return BinaryFormatter<T>::deserialize_from(buf.span());
        }

        template<typename T, size_t VBinarySize = sizeof(T)>
        Vector<T> read_many(size_t count)
        {
            Vector<T> buf = Vector<T>::create_with_size(count, true);
            for (size_t i = 0; i < count; i++)
                buf[i] = std::move(read<T, VBinarySize>().release_value());

            return buf;
        }

    private:
        InputStream& m_base_stream;
    };
}
using neo::BinaryReader;
