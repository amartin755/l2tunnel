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

#ifndef TCPSOCKET_HPP
#define TCPSOCKET_HPP

#include <stdexcept>
#include <string>
#include <cstdint>

#include "socketexception.hpp"


#if !HAVE_WINDOWS
    typedef int SOCKET;
    #define INVALID_SOCKET (-1)
#endif

class TcpSocket
{
public:
    TcpSocket () = delete;

    static TcpSocket connect (const std::string& host, uint16_t remotePort, bool ipv4 = true, bool ipv6 = true);
    static TcpSocket listen (uint16_t port, int backlog, bool ipv4 = true, bool ipv6 = true);
    void close () const;

    TcpSocket accept (std::string& addr, uint16_t& port) const;
    ssize_t recv (void *buf, size_t len) const;
    ssize_t send (const void *buf, size_t len) const;

    // get local address and port of socket
    std::string getsockname () const;
    // get remote address and port of socket
    std::string getpeername () const;

private:
    TcpSocket (SOCKET s);

    SOCKET m_socket;
};

#endif