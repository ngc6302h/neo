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
#include "Future.h"
#include <errno.h>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
namespace neo
{
    using SocketError = i32;

    class Ipv4SocketAddress
    {
    public:
        constexpr Ipv4SocketAddress(u32 ip_network_order, u16 port_network_order) :
            m_ip_network_order(ip_network_order), m_port_network_order(port_network_order)
        {
        }

        static Optional<Ipv4SocketAddress> from_string(String const& address)
        {
            auto ip_and_port = address.split(':');
            if (ip_and_port.is_empty() || ip_and_port.size() != 2)
                return {};

            u32 ipv4 {};
            if (::inet_pton(AF_INET, ip_and_port[0].null_terminated_characters(), &ipv4) != 1)
                return {};

            auto port = ip_and_port[1].checked_to<u16>();
            returnerr(port);
            return Ipv4SocketAddress { ipv4, htons(port.value()) };
        }

        constexpr u32 ip() const
        {
            return ntohl(m_ip_network_order);
        }

        constexpr u32 ip_in_network_order() const
        {
            return m_ip_network_order;
        }

        constexpr u16 port() const
        {
            return ntohs(m_port_network_order);
        }

        constexpr u16 port_in_network_order() const
        {
            return m_port_network_order;
        }

        constexpr Optional<String> to_string() const
        {
            char buf[INET_ADDRSTRLEN] {};
            if (::inet_ntop(AF_INET, &m_ip_network_order, buf, INET_ADDRSTRLEN) != buf)
                return {};
            return { buf };
        }

    private:
        u32 m_ip_network_order;
        u16 m_port_network_order;
    };

    class Ipv6SocketAddress
    {
    public:
        constexpr Ipv6SocketAddress(u128 ip_network_order, u16 port_network_order) :
            m_ip_network_order(ip_network_order), m_port_network_order(port_network_order)
        {
        }

        /// Turns an address in string representation to binary representation
        /// \param address The ipv6 address MUST be enclosed in brackets []. Example: [2001:db8::1]:8080
        static Optional<Ipv6SocketAddress> from_string(String const& address)
        {
            auto ip_begin = address.find("[");
            auto ip_end = address.find("]");

            if (ip_begin.is_end() || ip_end.is_end())
                return {};

            u128 ipv6 {};

            if (::inet_pton(AF_INET6, String(ip_begin, ip_end).null_terminated_characters(), &ipv6) != 1)
                return {};

            auto port = String(++ip_end, address.end()).checked_to<u16>();
            returnerr(port);
            return Ipv6SocketAddress { ipv6, htons(port.value()) };
        }

        constexpr u128 ip() const
        {
            return __builtin_bswap128(m_ip_network_order);
        }

        constexpr u128 ip_in_network_order() const
        {
            return m_ip_network_order;
        }

        constexpr u16 port() const
        {
            return ntohs(m_port_network_order);
        }

        constexpr u16 port_in_network_order() const
        {
            return m_port_network_order;
        }

        constexpr Optional<String> to_string() const
        {
            char buf[INET_ADDRSTRLEN] {};
            if (::inet_ntop(AF_INET, &m_ip_network_order, buf, INET_ADDRSTRLEN) != buf)
                return {};
            return { buf };
        }

    private:
        u128 m_ip_network_order;
        u16 m_port_network_order;
    };

    class TCPSocket
    {
    private:
        explicit TCPSocket(int socketfd, Ipv4SocketAddress client_address, Ipv4SocketAddress remote_address) :
            m_socketfd(socketfd),
            m_is_ipv4(true),
            m_ipv4_client_address(client_address),
            m_ipv4_remote_address(remote_address)
        {
        }

        explicit TCPSocket(int socketfd, Ipv6SocketAddress client_address, Ipv6SocketAddress remote_address) :
            m_socketfd(socketfd),
            m_is_ipv4(false),
            m_ipv6_client_address(client_address),
            m_ipv6_remote_address(remote_address)
        {
        }

    public:
        TCPSocket(TCPSocket const&) = delete;

        TCPSocket(TCPSocket&& other) :
            m_socketfd(other.m_socketfd)
        {
            other.m_socketfd = 0;
        }

        TCPSocket& operator=(TCPSocket const&) = delete;
        TCPSocket& operator=(TCPSocket&& other)
        {
            if (this == &other)
                return *this;

            new (this) TCPSocket(std::move(other));
            return *this;
        }

        /// \return A connected socket if success, or SocketError containing an errno code if error
        static ResultOrError<TCPSocket, SocketError> connect(Ipv4SocketAddress address)
        {
            auto maybe_socket_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (maybe_socket_fd == -1)
                return SocketError(errno);

            sockaddr_in socket_info {};
            socket_info.sin_addr.s_addr = address.ip_in_network_order();
            socket_info.sin_port = address.port_in_network_order();
            socket_info.sin_family = AF_INET;
            auto result = ::connect(maybe_socket_fd, reinterpret_cast<sockaddr const*>(&socket_info), sizeof(socket_info));
            if (result == -1)
                return SocketError(errno);

            [[maybe_unused]] socklen_t client_info_length {};
            sockaddr_in client_info {};
            if (getsockname(maybe_socket_fd, (sockaddr*)&client_info, &client_info_length) == -1)
                return SocketError(errno);

            TCPSocket socket { maybe_socket_fd, { client_info.sin_addr.s_addr, client_info.sin_port }, address };

            return socket;
        }

        /// \return A connected socket if success, or SocketError containing an errno code if error
        static ResultOrError<TCPSocket, SocketError> connect(Ipv6SocketAddress address)
        {
            auto maybe_socket_fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
            if (maybe_socket_fd == -1)
                return SocketError(errno);

            sockaddr_in6 socket_info {};
            *((u128*)&socket_info.sin6_addr) = address.ip_in_network_order();
            socket_info.sin6_port = address.port_in_network_order();
            socket_info.sin6_family = AF_INET6;
            auto result = ::connect(maybe_socket_fd, reinterpret_cast<sockaddr const*>(&socket_info), sizeof(socket_info));
            if (result == -1)
                return SocketError(errno);

            [[maybe_unused]] socklen_t client_info_length {};
            sockaddr_in6 client_info {};
            if (getsockname(maybe_socket_fd, (sockaddr*)&client_info, &client_info_length) == -1)
                return SocketError(errno);

            TCPSocket socket { maybe_socket_fd, { *(u128*)&client_info.sin6_addr, client_info.sin6_port }, address };

            return socket;
        }

        /// \param data Data to send
        /// \return The error that occurred, if any
        Optional<SocketError> send(Span<u8> const& data)
        {
            auto bytes_sent_or_error = ::send(m_socketfd, data.data(), data.size(), 0);

            if (bytes_sent_or_error == -1)
            {
                return SocketError(errno);
            }

            return {};
        }

        ResultOr<size_t, SocketError> receive(Span<u8>& buffer)
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

        Optional<SocketError> close()
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

        ResultOrError<bool, SocketError> data_available() const
        {
            fd_set readset;
            timeval timeout { 0, 0 };
            FD_ZERO(&readset);
            FD_SET(m_socketfd, &readset);

            auto result = ::select(1, &readset, nullptr, nullptr, &timeout);
            if (result == -1)
                return SocketError { errno };
            if (result == 1)
                return true;
            else
                return false;
        }

        bool ipv4() const
        {
            return m_is_ipv4;
        }

    private:

        int m_socketfd {};
        bool m_is_ipv4;
        union
        {
            Ipv6SocketAddress m_ipv6_client_address;
            Ipv4SocketAddress m_ipv4_client_address;
        };
        union
        {
            Ipv6SocketAddress m_ipv6_remote_address;
            Ipv4SocketAddress m_ipv4_remote_address;
        };
    };

    class UDPSocket
    {
    public:
        explicit UDPSocket(int socketfd, bool ipv4) :
            m_socketfd(socketfd),
            m_is_ipv4(ipv4)
        {
        }

        static ResultOrError<UDPSocket, SocketError> create(bool ipv4)
        {
            auto domain = ipv4 ? AF_INET : AF_INET6;
            auto maybe_socket_fd = ::socket(domain, SOCK_DGRAM, IPPROTO_UDP);
            if (maybe_socket_fd == -1)
                return SocketError(errno);

            return UDPSocket { maybe_socket_fd, ipv4 };
        }

        Optional<SocketError> send(Ipv4SocketAddress address, Span<u8> const& data)
        {
            sockaddr_in addr {};
            addr.sin_port = address.port_in_network_order();
            addr.sin_addr.s_addr = address.ip_in_network_order();
            auto bytes_sent_or_error = ::sendto(m_socketfd, data.data(), data.size(), 0, (sockaddr const*)&address, sizeof(addr));
            if (bytes_sent_or_error == -1)
                return SocketError { errno };
            return {};
        }

        Optional<SocketError> send(Ipv6SocketAddress address, Span<u8> const& data)
        {
            sockaddr_in6 addr {};
            addr.sin6_port = address.port_in_network_order();
            *(u128*)&addr.sin6_addr = address.ip_in_network_order();
            auto bytes_sent_or_error = ::sendto(m_socketfd, data.data(), data.size(), 0, (sockaddr const*)&address, sizeof(addr));
            if (bytes_sent_or_error == -1)
                return SocketError { errno };
            return {};
        }

        bool ipv4() const
        {
            return m_is_ipv4;
        }

        Optional<SocketError> close()
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

        ResultOrError<bool, SocketError> data_available() const
        {
            fd_set readset;
            timeval timeout { 0, 0 };
            FD_ZERO(&readset);
            FD_SET(m_socketfd, &readset);

            auto result = ::select(1, &readset, nullptr, nullptr, &timeout);
            if (result == -1)
                return SocketError { errno };
            if (result == 1)
                return true;
            else
                return false;
        }

    private:
        int m_socketfd {};
        bool m_is_ipv4;
        union
        {
            Ipv6SocketAddress m_ipv6_client_address;
            Ipv4SocketAddress m_ipv4_client_address;
        };
    };

}
using neo::TCPSocket;
