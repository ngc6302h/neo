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

namespace neo
{
    class BinaryWriter
    {
    public:
        explicit BinaryWriter(OutputStream& stream) :
            m_base_stream(stream)
        {
        }

        template<typename T, size_t VBinarySize = sizeof(T)>
        void write(T const& value)
        {
            Array<u8, sizeof(T)> buf;
            BinaryFormatter<T>::serialize_into(value, buf.span());
            m_base_stream.write(buf.span());
        }

        void flush()
        {
            m_base_stream.flush();
        }

        void close()
        {
            m_base_stream.close();
        }

    private:
        OutputStream& m_base_stream;
    };

    template<>
    void BinaryWriter::write<Span<u8>>(Span<u8> const& value)
    {
        m_base_stream.write(value);
    }
}
using neo::BinaryWriter;
