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
#include "Mutex.h"

namespace neo
{
    class ConcurrentInputStream : public InputStream
    {
    public:
        explicit ConcurrentInputStream(InputStream& base) :
            m_base(base), m_mutex()
        {
        }

        size_t read(Span<u8>& to) override
        {
            ScopedLock lock(m_mutex);
            return m_base.read(to);
        }

        size_t unlocked_read(Span<u8>& to)
        {
            return m_base.read(to);
        }

        bool end() const override
        {
            ScopedLock lock(m_mutex);
            return m_base.end();
        }

        bool unlocked_end() const
        {
            return m_base.end();
        }

        bool has_error() const override
        {
            ScopedLock lock(m_mutex);
            return m_base.has_error();
        }

        bool unlocked_has_error() const
        {
            return m_base.has_error();
        }

        void close() override
        {
            ScopedLock lock(m_mutex);
            m_base.close();
        }

        void unlocked_close()
        {
            ScopedLock lock(m_mutex);
            m_base.close();
        }

        bool is_locked() const
        {
            return m_mutex.is_locked();
        }

    private:
        InputStream& m_base;
        mutable Mutex m_mutex;
    };

    class ConcurrentOutputStream : public OutputStream
    {
    public:
        explicit ConcurrentOutputStream(OutputStream& base) :
            m_base(base), m_mutex()
        {
        }

        void write(Span<const u8> const& from) override
        {
            ScopedLock lock(m_mutex);
            m_base.write(from);
        }

        void unlocked_write(Span<const u8> const& from)
        {
            m_base.write(from);
        }

        void flush() override
        {
            ScopedLock lock(m_mutex);
            m_base.flush();
        }

        void unlocked_flush()
        {
            m_base.flush();
        }

        bool has_error() const override
        {
            ScopedLock lock(m_mutex);
            return m_base.has_error();
        }

        bool unlocked_has_error() const
        {
            return m_base.has_error();
        }

        void close() override
        {
            ScopedLock lock(m_mutex);
            m_base.close();
        }

        void unlocked_close()
        {
            m_base.close();
        }

        bool is_locked() const
        {
            return m_mutex.is_locked();
        }

    private:
        OutputStream& m_base;
        mutable Mutex m_mutex;
    };
}
using neo::ConcurrentInputStream;
using neo::ConcurrentOutputStream;
