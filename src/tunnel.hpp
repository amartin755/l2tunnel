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

#ifndef TUNNEL_HPP
#define TUNNEL_HPP

#include <cstdint>

static inline uint32_t swap32 (uint32_t val)
{
#if HAVE_BIG_ENDIAN
  return (((val & 0xff000000u) >> 24) | ((val & 0x00ff0000u) >> 8)	\
   | ((val & 0x0000ff00u) << 8) | ((val & 0x000000ffu) << 24));
#else
    return val;
#endif
}
static inline uint16_t swap16 (uint16_t val)
{
#if HAVE_BIG_ENDIAN
    return ((__uint16_t) (((val >> 8) & 0xff) | ((val & 0xff) << 8)));
#else
    return val;
#endif
}

enum Type : uint16_t {
    NOP = 0,            // don't do anything
    HELLO = 0x326C,     // establish connection (client --HELLO-> server --HELLO-> client)
    PACKET = 1        // encapsulated Ethernet packet
};

struct TunnelHeader
{
    static void* packet (uint8_t* buf, uint32_t payloadLength)
    {
        TunnelHeader* h = (TunnelHeader*)buf;
        h->m_res = 0;
        h->setType (Type::PACKET);
        h->setLength (payloadLength);
        return buf;
    }

    Type getType () const
    {
        return (Type)swap16 (m_type);
    }
    void setType (Type t)
    {
        m_type = (Type)swap16 (t);
    }
    uint32_t getLength () const
    {
        return swap32 (m_len);
    }
    void setLength (uint32_t len)
    {
        m_len = swap32 (len);
    }
    bool isPacket () const
    {
        return getType() == Type::PACKET;
    }

private:
    Type m_type;
    uint16_t m_res;
    uint32_t m_len;
};

// ensure packet struct without using compiler specific packing attributes/pragmas
static_assert (sizeof (struct TunnelHeader) == 8, "TunnelHeader is not natural aligned");

#endif