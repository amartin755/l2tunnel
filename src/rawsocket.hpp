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

#ifndef RAWSOCKET_HPP
#define RAWSOCKET_HPP

#include <stdexcept>
#include <string>
#include <cstdint>

#include "socketexception.hpp"
#include "sockettype.h"
#include "socketevent.hpp"


#if USE_PCAP
    typedef pcap_t* RAW_SOCKET;
    #define INVALID_RAWSOCKET (nullptr)
#else
    typedef int RAW_SOCKET;
    #define INVALID_RAWSOCKET (-1)
#endif

class RawSocket
{
public:
    RawSocket () = delete;
    RawSocket (const RawSocket&) = delete;
    RawSocket& operator=(const RawSocket&) = delete;
    RawSocket& operator=(const RawSocket&&) = delete;

    RawSocket (RawSocket&& obj);
    ~RawSocket ();

    static RawSocket open (const std::string& interface);
    void close ();

    size_t recv (void *buf, size_t len) const;
    size_t send (const void *buf, size_t len) const;

    bool isValid () const
    {
        return m_socket != INVALID_RAWSOCKET;
    }

    void cancel () const;

private:
    RawSocket (RAW_SOCKET s);

    RAW_SOCKET m_socket;
    SocketEvent m_event;
};

#endif