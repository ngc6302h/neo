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
#include "ResultOrError.h"
#include "Span.h"
#include "String.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace neo
{

    using SocketError = i32;
    
    class Socket
    {
    public:
        virtual Optional<SocketError> send(Span<u8> const& data) = 0;
        virtual ResultOr<size_t, SocketError> receive(Span<u8>& buffer) = 0;
        virtual Optional<SocketError> close() = 0;
    };
    
    class TCPSocket : public Socket
    {
    private:
        explicit TCPSocket(int socketfd, u32 client_ipv4_address_in_network_order, u16 client_port_in_network_order, u32 remote_ipv4_address_in_network_order, u16 remote_port_in_network_order) :
            m_socketfd(socketfd),
            m_client_ipv4_network_order(client_ipv4_address_in_network_order),
            m_client_port_network_order(client_port_in_network_order),
            m_remote_ipv4_network_order(remote_ipv4_address_in_network_order),
            m_remote_port_network_order(remote_port_in_network_order)
        {}
    
    public:
        TCPSocket(TCPSocket const&) = delete;
        
        TCPSocket(TCPSocket&& other) : m_socketfd(other.m_socketfd), m_client_ipv4_network_order(other.m_client_ipv4_network_order), m_client_port_network_order(other.m_client_port_network_order),
            m_remote_ipv4_network_order(other.m_remote_ipv4_network_order), m_remote_port_network_order(other.m_remote_port_network_order)
        {
            other.m_socketfd = 0;
            other.m_client_ipv4_network_order = 0;
            other.m_client_port_network_order = 0;
            other.m_remote_ipv4_network_order = 0;
            other.m_remote_port_network_order = 0;
        }
        
        TCPSocket& operator=(TCPSocket const&) = delete;
        TCPSocket& operator=(TCPSocket&& other)
        {
            if (this == &other)
                return *this;
            
            new (this) TCPSocket(std::move(other));
            return *this;
        }
        
        /// \param ip Must be a valid string representation of an IPv4 address
        /// \param port Must be in host order
        /// \return A connected socket if success, or SocketError containing an errno code if error
        static ResultOrError<TCPSocket, SocketError> connect(String const& ip, u16 port)
        {
            in_addr addr {};
            if (::inet_aton(ip.null_terminated_characters(), &addr) == 0)
            {
                return SocketError(errno);
            }
            
            auto maybe_socket_no = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (maybe_socket_no == -1)
            {
                return SocketError(errno);
            }
            
            sockaddr_in socket_info {};
            socket_info.sin_addr = addr;
            socket_info.sin_port = htons(port);
            socket_info.sin_family = AF_INET;
            auto result = ::connect(maybe_socket_no, reinterpret_cast<sockaddr const*>(&socket_info), sizeof(socket_info));
            if (result == -1)
            {
                return SocketError(errno);
            }
            
            [[maybe_unused]] socklen_t client_info_length {};
            sockaddr_in client_info {};
            if (getsockname(maybe_socket_no, (sockaddr*)&client_info, &client_info_length) == -1)
            {
                return SocketError(errno);
            }
            
            TCPSocket socket {maybe_socket_no, client_info.sin_addr.s_addr, client_info.sin_port, socket_info.sin_addr.s_addr, socket_info.sin_port};
            
            return socket;
        }
        
        /// \param data Data to send
        /// \return The error that occurred, if any
        virtual Optional<SocketError> send(Span<u8> const& data) override
        {
            auto bytes_sent_or_error = ::send(m_socketfd, data.data(), data.size(), 0);
            
            if (bytes_sent_or_error == -1)
            {
                return SocketError(errno);
            }
            
            return {};
        }
        
        virtual ResultOr<size_t, SocketError> receive(Span<u8>& buffer) override
        {
            VERIFY(buffer.size() != 0);
            VERIFY(buffer.data() != nullptr);
            
            auto bytes_read = ::recv(m_socketfd, buffer.data(), buffer.size(), 0);
            if (bytes_read == -1)
            {
                return SocketError(errno);
            }
            
            return (size_t)bytes_read;
        }
        
        virtual Optional<SocketError> close() override
        {
            if (::close(m_socketfd) == -1)
            {
                return SocketError(errno);
            }
            
            m_socketfd = 0;
            return {};
        }
        
        bool is_open() const
        {
            return m_socketfd != 0;
        }
        
    private:
        
        int m_socketfd {};
        u32 m_client_ipv4_network_order {};
        u16 m_client_port_network_order {};
        u32 m_remote_ipv4_network_order {};
        u16 m_remote_port_network_order {};
    };
    
}
using neo::Socket;