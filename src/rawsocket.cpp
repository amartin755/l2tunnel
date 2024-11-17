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

#include <unistd.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <cstring>

#include "rawsocket.hpp"
#include "bug.hpp"

RawSocket::RawSocket (RAW_SOCKET s) : m_socket (s)
{
}

RawSocket::RawSocket (RawSocket&& obj)
{
    m_socket = obj.m_socket;
    obj.m_socket = INVALID_RAWSOCKET;
}

RawSocket::~RawSocket ()
{
    if (m_socket != INVALID_RAWSOCKET)
    {
        // we directly call close becaus RawSocket::close might throw an exception
        ::close (m_socket);
        m_socket = INVALID_RAWSOCKET;
    }
}

RawSocket RawSocket::open (const std::string& interface)
{
    int ifIndex = if_nametoindex (interface.c_str ());
    if (!ifIndex)
        throw SocketException();

    RawSocket s (socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL)));
    if (!s.isValid())
        throw SocketException();

    struct sockaddr_ll sll;
    std::memset (&sll, 0, sizeof (sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = ifIndex;

    if (bind(s.m_socket, (struct sockaddr *)&sll, sizeof(sll)) < 0)
        throw SocketException();

    return s;
}

void RawSocket::close ()
{
    if (::close (m_socket))
    {
        throw SocketException();
    }
    m_socket = INVALID_RAWSOCKET;
}

void RawSocket::cancel () const
{
    m_event.cancel ();
}

size_t RawSocket::recv (void *buf, size_t len) const
{
    if (!m_event.waitRecv (m_socket))
        return 0; // if no timeout is provided, this must not happen

    auto ret = ::recv (m_socket, buf, len, 0); // auto because on windows the return value is int

    if (ret <= 0)
        throw SocketException ();

    return (size_t)ret;
}

size_t RawSocket::send (const void *buf, size_t len) const
{
    auto ret = ::send (m_socket, buf, len, 0); // auto because on windows the return value is int
    if (ret <= 0)
        throw SocketException ();

    // if no error is returned, the sent length must match the expected lenght
    BUG_ON ((size_t)ret != len);

    return (size_t)ret;
}
