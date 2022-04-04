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
#include "ResultOrError.h"
#include "OSError.h"
#include "String.h"
#include "Iterator.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace neo
{
    enum class FileType : u8
    {
        Unknown = DT_UNKNOWN,
        FIFO = DT_FIFO,
        CharDevice = DT_CHR,
        Directory = DT_DIR,
        BlockDevice = DT_BLK,
        File = DT_REG,
        SymLink = DT_LNK,
        Socket = DT_SOCK
    };

    class DirectoryIterator;
    class Directory;

    class DirEntry
    {
        friend DirectoryIterator;
        DirEntry(String&& name, u64 inode_number, FileType filetype) :
            m_name(move(name)), m_inode_num(inode_number), m_type(filetype)
        {
        }

    public:
        DirEntry(DirEntry const&) = default;
        DirEntry(DirEntry&& other) :
            m_name(move(other.m_name)), m_inode_num(other.m_inode_num), m_type(other.m_type)
        {
        }

        DirEntry& operator=(DirEntry const&) = default;
        DirEntry& operator=(DirEntry&& other)
        {
            if (this == &other)
                return *this;

            new (this) DirEntry(move(other));

            return *this;
        }

        String& name()
        {
            return m_name;
        }

        String const& name() const
        {
            return m_name;
        }

        GENERATE_GETTER_BY_VALUE(m_inode_num, inode_number)

        GENERATE_GETTER_BY_VALUE(m_type, filetype)

    private:
        String m_name;
        u64 m_inode_num;
        FileType m_type;
    };

    class DirectoryIterator
    {
        friend class Directory;

        DirectoryIterator(DIR* stream) :
            m_stream(stream)
        {
        }

    public:
        DirectoryIterator(DirectoryIterator&& other)
        {
            if (m_stream != nullptr)
                closedir(m_stream);
            m_stream = other.m_stream;
            other.m_stream = nullptr;
        }

        DirectoryIterator& operator=(DirectoryIterator&& other)
        {
            if (this == &other)
                return *this;

            new (this) DirectoryIterator(move(other));

            return *this;
        }

        ResultOrError<DirEntry, OSError> next()
        {
            errno = 0;
            auto result = readdir(m_stream);
            if (result == nullptr)
                return OSError(errno);

            return DirEntry(String((char*)result->d_name), (u64)result->d_ino, FileType(result->d_type));
        }

        ~DirectoryIterator()
        {
            if (m_stream != nullptr)
                closedir(m_stream);
        }

        DIR* m_stream;
    };

    class Directory
    {
    public:
        static ResultOrError<Directory, OSError> open(String const& path)
        {
            auto fd = openat(AT_FDCWD, path.null_terminated_characters(), O_RDONLY | O_DIRECTORY);
            if (fd == -1)
                return OSError(errno);

            return Directory(fd, path);
        }

        static Optional<OSError> create(String const& path, mode_t mode)
        {
            auto result = mkdirat(AT_FDCWD, path.null_terminated_characters(), mode);
            if (result == -1)
                return OSError(errno);

            return {};
        }

        [[nodiscard]] ResultOrError<DirectoryIterator, OSError> iterator() const
        {
            DIR* stream = opendir(m_path.null_terminated_characters());
            if (stream == nullptr)
                return OSError(errno);

            return DirectoryIterator(stream);
        }

    private:
        explicit Directory(int fd, String const& path) :
            m_fd(fd), m_path(path) { }
        int m_fd;
        String m_path;
    };
}
using neo::Directory;
