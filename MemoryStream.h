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
#include "Vector.h"
#include "Stream.h"
#include "Checked.h"

namespace neo
{
    class MemoryStream final : public InputStream, OutputStream
    {
    public:
        explicit MemoryStream(size_t initial_size) :
            m_backing(initial_size, true), m_read_pos(), m_write_pos()
        {
        }

        virtual size_t read(Span<u8>& to) override
        {
            size_t to_read = min(to.size(), m_backing.size() - m_read_pos);
            UntypedCopy(to_read, m_backing.data(), to.data());
            m_read_pos += to_read;
            return to_read;
        }

        virtual void write(Span<const u8> const& from) override
        {
            m_backing.append(from);
            m_write_pos += from.size();
        }

        virtual void flush() override
        {
            return;
        }

        virtual bool end() const override
        {
            return m_backing.size() == m_read_pos;
        }

        virtual void close() override
        {
            return;
        }

        Vector<u8> release_buffer()
        {
            return move(m_backing);
        }

        void seek(size_t pos)
        {
            VERIFY(pos <= m_backing.size());
            m_read_pos = pos;
        }

        void copy_to(Span<u8>& buffer)
        {
            VERIFY(buffer.size() >= m_backing.size());
            Copy(m_backing.size(), m_backing.data(), buffer.data());
        }

        size_t size() const
        {
            return m_write_pos;
        }

        size_t unread_bytes() const
        {
            if (m_read_pos == m_write_pos)
                return 0;

            return m_write_pos - m_read_pos;
        }

        virtual bool has_error() const override
        {
            return false;
        }

    private:
        Vector<u8> m_backing;
        size_t m_read_pos;
        size_t m_write_pos;
    };
}
using neo::MemoryStream;
