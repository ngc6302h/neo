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
#include "File.h"
#include "ResultOrError.h"

namespace neo
{
    class FileStream final : public InputStream, OutputStream
    {
    public:
        explicit FileStream(File&& open_file) :
            m_file(move(open_file)), m_last_error(OSError::Success)
        {
        }

        static ResultOrError<FileStream, OSError> create(StringView filename, StringView posix_open_mode)
        {
            auto maybe_file = File::open(filename, posix_open_mode.non_null_terminated_buffer());
            if (maybe_file.has_error())
                return maybe_file.error();
            return FileStream { move(maybe_file.result()) };
        }

        void close() override
        {
            auto maybe_error = m_file.close();
            if (maybe_error.has_value())
                m_last_error = maybe_error.release_value();
        }

        bool has_error() const override
        {
            return m_file.has_error() || m_last_error != OSError::Success;
        }

        size_t read(Span<u8>& to) override
        {
            auto maybe_bytes_read = m_file.read(to, to.size());
            if (maybe_bytes_read.has_error())
                m_last_error = maybe_bytes_read.error();
            return maybe_bytes_read.result_or(0);
        }

        bool end() const override
        {
            return m_file.eof();
        }

        void write(Span<const u8> const& from) override
        {
            auto maybe_error = m_file.write(from, from.size());
            if (maybe_error.has_error())
                m_last_error = maybe_error.error();
        }

        void flush() override
        {
            return;
        }

        File& file()
        {
            return m_file;
        }

        ResultOrError<long, OSError> pos() const
        {
            return m_file.getpos();
        }

        OSError last_error() const
        {
            return m_last_error;
        }

        void clear_error_state()
        {
            m_last_error = OSError::Success;
        }

        Optional<OSError> seek(long offset)
        {
            auto maybe_error = m_file.seek(SeekMode::Start, offset);
            if (maybe_error.has_value())
                m_last_error = maybe_error.value();
            return maybe_error;
        }

    private:
        File m_file;
        OSError m_last_error;
    };
}
using neo::FileStream;
