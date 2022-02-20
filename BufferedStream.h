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
#include "Stream.h"
#include "Vector.h"

namespace neo
{
    class BufferedStream final : public OutputStream
    {
        explicit BufferedStream(OutputStream& base, size_t buffer_size) :
            m_base(base), m_buffer(buffer_size, true)
        {
        }

        virtual void write(Span<const u8> const& from) override
        {
            if (m_buffer.size() + from.size() > m_buffer.capacity())
                flush();
            UntypedCopy(from.size(), from.data(), m_buffer.data());
        }

        virtual void flush() override
        {
            m_base.write(m_buffer.span().as_readonly());
            m_buffer.clear();
        }

        virtual void close() override
        {
            m_base.close();
        }

        virtual bool has_error() const override
        {
            return m_base.has_error();
        }

    private:
        OutputStream& m_base;
        Vector<u8> m_buffer;
    };
}
using neo::BufferedStream;
