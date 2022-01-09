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
#include "Error.h"
#include "Optional.h"
#include "ResultOrError.h"
#include "Span.h"
#include "String.h"
#include "Vector.h"
#include <stdio.h>
#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

namespace neo
{
    enum class SeekMode
    {
        Start = SEEK_SET,
        Current = SEEK_CUR,
        End = SEEK_END
    };

    // TODO: Implement reporting of errors
    class File
    {
    public:
        ~File()
        {
            if (m_is_open)
                fclose(m_handle);
        }

        File(File&& other) :
            m_handle(other.m_handle), m_is_open(other.m_is_open)
        {
            other.m_handle = nullptr;
            other.m_is_open = false;
        }

        File& operator=(File&& other)
        {
            m_handle = other.m_handle;
            m_is_open = other.m_is_open;
            other.m_handle = nullptr;
            other.m_is_open = false;
            return *this;
        }

        [[nodiscard]] static Optional<Error> remove(const String& path)
        {
            auto result = ::remove(path.null_terminated_characters());
            if (result != 0)
                return (Error)errno;
            return {};
        }

        [[nodiscard]] static bool rename(const StringView& path, const String& new_path)
        {
            return ::rename(path.non_null_terminated_buffer(), new_path.null_terminated_characters()) == 0;
        }

        [[nodiscard]] static Optional<Error> exists(const StringView& path)
        {
#ifdef _WIN32
    #pragma warning(suppress : 4996)
#endif
            auto success = access(path.non_null_terminated_buffer(), F_OK);
            if (success == 0)
                return {};
            else
                return (Error)errno;
        }

        [[nodiscard]] static ResultOrError<File, Error> open(const String& path, const char* posix_open_mode)
        {
            VERIFY(!path.is_empty());

            FILE* file = fopen(path.null_terminated_characters(), posix_open_mode);
            if (file == nullptr)
                return (Error)errno;
            return File(file);
        }

        [[nodiscard]] static ResultOrError<Vector<u8>, Error> read_to_buffer(const String& path, size_t max_bytes_to_read)
        {
            VERIFY(!path.is_empty());

            Vector<u8> buffer((size_t)max_bytes_to_read, true);

            auto file_or_error = open(path, "r");
            if (file_or_error.has_error())
                return file_or_error.error();

            auto file = move(file_or_error.result());
            auto result_or_error = file.read(buffer.span(), max_bytes_to_read);
            if (result_or_error.has_error())
                return result_or_error.error();
            auto maybe_error = file.close();
            if (maybe_error.has_value())
                return maybe_error.value();
            buffer.change_capacity(result_or_error.result());
            return buffer;
        }

        [[nodiscard]] static ResultOrError<Vector<u8>, Error> read_all(const String& path)
        {
            auto file_or_error = open(path, "r");
            if (file_or_error.has_error())
                return (Error)errno;
            auto size_or_error = file_or_error.result().size();
            if (size_or_error.has_error())
                return (Error)errno;
            Vector<u8> buffer((size_t)size_or_error.result(), true);
            auto bytes_read_or_error = file_or_error.result().read(buffer.span(), buffer.size());
            if (bytes_read_or_error.has_error())
                return (Error)errno;
            return buffer;
        }

        [[nodiscard]] ResultOrError<u8, Error> read_byte()
        {
            if (!m_is_open)
                return Error::BadFileNumber;

            auto byte_read = fgetc(m_handle);
            if (byte_read == EOF)
                return Error::EndOfFile;
            return (u8)byte_read;
        }

        [[nodiscard]] ResultOrError<size_t, Error> read(const Span<u8>& to, size_t max_bytes)
        {
            VERIFY(to.size() <= max_bytes);
            if (!m_is_open)
                return Error::BadFileNumber;

            size_t bytes_read = fread(to.data(), 1, max_bytes, m_handle);

            if (bytes_read == 0)
            {
                if (feof(m_handle))
                    return Error::EndOfFile;
                else
                    return (Error)errno;
            }
            return bytes_read;
        }

        [[nodiscard]] ResultOrError<size_t, Error> write(const Span<const u8> from, size_t bytes_to_write)
        {
            VERIFY(bytes_to_write <= from.size());
            if (!m_is_open)
                return Error::BadFileNumber;

            size_t bytes_written = fwrite(from.data(), 1, bytes_to_write, m_handle);
            return bytes_written;
        }

        [[nodiscard]] Optional<Error> seek(SeekMode relative_to, long offset)
        {
            auto result = fseek(m_handle, offset, (int)relative_to);
            if (result == -1)
                return (Error)errno;
            return {};
        }

        [[nodiscard]] Optional<Error> close()
        {
            if (fclose(m_handle) == EOF)
                return (Error)errno;
            m_is_open = false;
            return {};
        }

        [[nodiscard]] ResultOrError<long, Error> getpos() const
        {
            auto result = ftell(m_handle);
            if (result == -1)
                return (Error)errno;
            return result;
        }

        [[nodiscard]] bool eof() const
        {
            return feof(m_handle);
        }

        [[nodiscard]] bool has_error() const
        {
            return ferror(m_handle);
        }

        [[nodiscard]] static ResultOrError<long, Error> size(const String& path)
        {
            auto* handle = fopen(path.null_terminated_characters(), "r");
            auto result = fseek(handle, 0, SEEK_END);
            if (result == -1)
            {
                fclose(handle);
                return (Error)errno;
            }
            auto size = ftell(handle);
            if (size == -1)
            {
                fclose(handle);
                return (Error)errno;
            }
            fclose(handle);
            return size;
        }

        [[nodiscard]] ResultOrError<long, Error> size() const
        {
            auto current_pos = ftell(m_handle);
            if (current_pos == -1)
                return (Error)errno;
            auto result = fseek(m_handle, 0, SEEK_END);
            if (result == -1)
                return (Error)errno;
            auto size = ftell(m_handle);
            if (size == -1)
                return (Error)errno;
            result = fseek(m_handle, current_pos, SEEK_SET);
            if (result == -1)
                return (Error)errno;
            return size;
        }

    private:
        explicit File(_IO_FILE* handle) :
            m_handle(handle), m_is_open(true)
        {
        }

        _IO_FILE* m_handle { nullptr };
        bool m_is_open { false };
    };
}
using neo::File;
using neo::SeekMode;
