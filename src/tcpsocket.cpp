// SPDX-License-Identifier: GPL-3.0-only
/*
 * l2tunnel <https://github.com/amartin755/l2tunnel>
 * Copyright (C) 2024 Andreas Martin (netnag@mailbox.org)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#if HAVE_WINDOWS
#include <Winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <cstring>
#include <sstream>
#include <list>

#include "tcpsocket.hpp"

// ------------ local helper functions ------------
static std::string ipToString (const struct sockaddr* addr);
    struct info
    {
        info (const struct addrinfo& info)
        {
            family = info.ai_family;
            socktype = info.ai_socktype;
            protocol = info.ai_protocol;
            addrlen = info.ai_addrlen;
            std::memcpy (&addr, info.ai_addr, addrlen);
        }
        int              family;
        int              socktype;
        int              protocol;
        socklen_t        addrlen;
        struct sockaddr_storage  addr;
    };
static void getaddrinfo (const std::string& node, uint16_t remotePort,
    int family, int sockType, int protocol, std::list<info> &result);


TcpSocket::TcpSocket (SOCKET s) : m_socket (s)
{
}

TcpSocket::TcpSocket (TcpSocket&& obj)
{
    m_socket = obj.m_socket;
    obj.m_socket = INVALID_SOCKET;
}

TcpSocket::~TcpSocket ()
{
    if (m_socket != INVALID_SOCKET)
    {
        // we directly call close becaus TcpSocket::close might throw an exception
        ::close (m_socket);
        m_socket = INVALID_SOCKET;
    }
}

void TcpSocket::close ()
{
    if (::close (m_socket))
    {
        throw SocketException();
    }
    m_socket = INVALID_SOCKET;
}

TcpSocket TcpSocket::connect (const std::string& host, uint16_t remotePort, bool ipv4, bool ipv6)
{
    std::list <struct info> r;
    int family = ipv4 && ipv6 ? AF_UNSPEC : (ipv6 ? AF_INET6 : AF_INET);
    ::getaddrinfo (host, remotePort, family, SOCK_STREAM, 0, r);
    for (const auto& addrInfo : r)
    {
        SOCKET s = socket (addrInfo.family, addrInfo.socktype, addrInfo.protocol);
        if (s == INVALID_SOCKET)
            throw SocketException();

        if (::connect (s, (sockaddr*)&addrInfo.addr, addrInfo.addrlen))
        {
            ::close (s);
            continue;
        }

        return TcpSocket(s); // success
    }

    throw SocketException (std::string("Could not connect to ") + host);
}

TcpSocket TcpSocket::listen (uint16_t port, int backlog, bool ipv4, bool ipv6)
{
    TcpSocket s (socket (ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0));
    if (!s.isValid())
        throw SocketException();

    const int enable = 1;
    if (::setsockopt (s.m_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
        throw SocketException ();

    const int ipv6only = !ipv4 && ipv6;
    if (::setsockopt (s.m_socket, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(ipv6only)))
        throw SocketException ();

    struct sockaddr_storage address;
    struct sockaddr_in*  addr4 = (struct sockaddr_in*)&address;
    struct sockaddr_in6* addr6 = (struct sockaddr_in6*)&address;
    std::memset (&address, 0, sizeof (address));
    if (!ipv6)
    {
        addr4->sin_family      = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port        = htons (port);
    }
    else
    {
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr   = in6addr_any;
        addr6->sin6_port   = htons (port);
    }

    if (::bind (s.m_socket, (struct sockaddr *) &address, sizeof (address)))
        throw SocketException ();

    if (::listen (s.m_socket, backlog))
        throw SocketException ();

    return s;
}

TcpSocket TcpSocket::accept (std::string& addr, uint16_t& port) const
{
    struct sockaddr_storage address;
    std::memset (&address, 0, sizeof(address));
    socklen_t addrLen = sizeof (address);

    int ret = ::accept (m_socket, (struct sockaddr *)&address, &addrLen);
    if (ret < 0)
        throw SocketException ();

    addr = ipToString ((struct sockaddr *)&address);
    port = ntohs (((struct sockaddr_in6*)&address)->sin6_port);

    return TcpSocket (ret);
}

ssize_t TcpSocket::recv (void *buf, size_t len) const
{
    auto ret = ::recv (m_socket, buf, len, 0); // auto because on windows the return value is int
    if (ret < 0)
        throw SocketException ();

    return (ssize_t)ret;
}

ssize_t TcpSocket::send (const void *buf, size_t len) const
{
    auto ret = ::send (m_socket, buf, len, 0); // auto because on windows the return value is int
    if (ret < 0)
        throw SocketException ();

    return (ssize_t)ret;
}

std::string TcpSocket::getsockname () const
{
    std::ostringstream out;
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (::getsockname (m_socket, (struct sockaddr *) &addr, &len))
        throw SocketException ();

    out << ipToString ((struct sockaddr *) &addr) << ":" << ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
    return out.str();
}

std::string TcpSocket::getpeername () const
{
    std::ostringstream out;
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (::getpeername (m_socket, (struct sockaddr *) &addr, &len))
        throw SocketException ();

    out << ipToString ((struct sockaddr *) &addr) << ":" << ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
    return out.str();
}


// ------------ local helper functions ------------

std::string ipToString   (const struct sockaddr* addr)
{
    const char* ret = "";
    if (addr->sa_family == AF_INET)
    {
        char ip[INET_ADDRSTRLEN];
        ret = ::inet_ntop(AF_INET, &((struct sockaddr_in *)addr)->sin_addr , ip, sizeof(ip));
    }
    if (addr->sa_family == AF_INET6)
    {
        char ip[INET6_ADDRSTRLEN];
        ret = ::inet_ntop(AF_INET6, &((struct sockaddr_in6 *)addr)->sin6_addr , ip, sizeof(ip));
    }

    if (!ret)
        throw SocketException ();

    return ret;
}

void getaddrinfo (const std::string& node, uint16_t remotePort,
    int family, int sockType, int protocol, std::list<info>& result)
{
    struct addrinfo hints;
    struct addrinfo *res, *rp;

    std::memset (&hints, 0, sizeof (hints));
    hints.ai_family   = family;
    hints.ai_socktype = sockType;
    hints.ai_protocol = protocol;
    result.clear ();

    int s = ::getaddrinfo (node.c_str (), 
        sockType != SOCK_RAW ? std::to_string(remotePort).c_str() : NULL, 
        &hints, &res);
    if (s != 0)
    {
        throw SocketException (gai_strerror(s));
    }
    for (rp = res; rp != NULL; rp = rp->ai_next)
    {
        result.emplace_back (*rp);
    }
    freeaddrinfo (res);
}
