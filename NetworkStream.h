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

#include "Socket.h"
#include "Stream.h"

namespace neo
{
    class NetworkStream final : public InputStream, public OutputStream
    {
    public:
        virtual size_t read(Span<u8>& to) override
        {
            auto result = m_socket->receive(to);
            if (result.has_error())
            {
                m_last_error = result.error();
                return 0;
            }
            m_last_error = 0;
            return result.result();
        }

        virtual bool end() const override
        {
            // Check if the socket can read
            return false;
        }

        virtual void write(Span<u8> const& from) override
        {
            auto error = m_socket->send(from);
            if (error.has_value())
            {
                m_last_error = error.value();
                return;
            }
            m_last_error = 0;
        }

        virtual void flush() override
        {
        }

        virtual void close() override
        {
            auto error = m_socket->close();
            if (error.has_value())
            {
                m_last_error = error.value();
                return;
            }
            m_last_error = 0;
        }

        virtual bool has_error() const override
        {
            return m_last_error != 0;
        }

    private:
        mutable OwnPtr<Socket> m_socket;
        SocketError m_last_error { 0 };
    };

}
using neo::NetworkStream;
