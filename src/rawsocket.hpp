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


#if USE_PCAP
    typedef pcap_t* RAW_SOCKET;
#else
    typedef int RAW_SOCKET;
#endif

class RawSocket
{
public:
    RawSocket () = delete;

    static RawSocket open (const std::string& interface);
    void close () const;

    ssize_t recv (void *buf, size_t len) const;
    ssize_t send (const void *buf, size_t len) const;

private:
    RawSocket (RAW_SOCKET s);

    RAW_SOCKET m_socket;
};

#endif