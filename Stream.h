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
#include "Span.h"

namespace neo
{
    class Stream
    {
    public:
        virtual void close() = 0;
        virtual bool has_error() const = 0;
    };

    class InputStream : public Stream
    {
    public:
        virtual size_t read(Span<u8>& to) = 0;
        virtual bool end() const = 0;
    };

    class OutputStream : public Stream
    {
    public:
        virtual void write(Span<const u8> const& from) = 0;
        virtual void flush() = 0;
    };
}
using neo::InputStream;
using neo::OutputStream;
using neo::Stream;
