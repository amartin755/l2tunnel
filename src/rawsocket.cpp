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


RawSocket::RawSocket (RAW_SOCKET s) : m_socket (s)
{
}

RawSocket RawSocket::open (const std::string& interface)
{
    RAW_SOCKET s = socket (PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (s < 0)
        throw SocketException();

    int ifIndex = if_nametoindex (interface.c_str ());
    if (!ifIndex)
        throw SocketException();

    struct sockaddr_ll sll;
    std::memset (&sll, 0, sizeof (sll));
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = ifIndex;
    
    if (bind(s, (struct sockaddr *)&sll, sizeof(sll)) < 0)
        throw SocketException();

    return RawSocket (s);
}

void RawSocket::close () const
{
    if (::close (m_socket))
    {
        throw SocketException();
    }
}

ssize_t RawSocket::recv (void *buf, size_t len) const
{
    auto ret = ::recv (m_socket, buf, len, 0); // auto because on windows the return value is int
    if (ret < 0)
        throw SocketException ();

    return (ssize_t)ret;
}

ssize_t RawSocket::send (const void *buf, size_t len) const
{
    auto ret = ::send (m_socket, buf, len, 0); // auto because on windows the return value is int
    if (ret < 0)
        throw SocketException ();

    return (ssize_t)ret;
}
