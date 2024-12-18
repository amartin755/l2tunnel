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
#include "sockettype.h"
#include "socketevent.hpp"


class TcpSocket
{
public:
    TcpSocket () = delete;
    TcpSocket (const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&) = delete;
    TcpSocket& operator=(const TcpSocket&&) = delete;

    TcpSocket (TcpSocket&& obj);
    ~TcpSocket ();

    static TcpSocket connect (const std::string& host, uint16_t remotePort, bool ipv4 = true, bool ipv6 = true);
    static TcpSocket listen (uint16_t port, int backlog, bool ipv4 = true, bool ipv6 = true);
    void close ();

    TcpSocket accept (std::string& addr, uint16_t& port) const;
    size_t recv (void *buf, size_t len) const;
    size_t send (const void *buf, size_t len) const;

    // get local address and port of socket
    std::string getsockname () const;
    // get remote address and port of socket
    std::string getpeername () const;

    bool isValid () const
    {
        return m_socket != INVALID_SOCKET;
    }

    void cancel () const;

private:
    TcpSocket (SOCKET s);

    SOCKET m_socket;
    SocketEvent m_event;
};

#endif