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

#ifndef SOCKETEVENT_HPP
#define SOCKETEVENT_HPP

#include "sockettype.h"

typedef int EVENT;
#define INVALID_EVENT (-1)

class SocketEvent
{
public:
    SocketEvent ();
    SocketEvent (const SocketEvent&) = delete;
    SocketEvent& operator=(const SocketEvent&) = delete;
    SocketEvent& operator=(const SocketEvent&&) = delete;
    SocketEvent (SocketEvent&& obj);
    ~SocketEvent ();

    bool wait (SOCKET s, bool &recv, bool &send, int timeout = -1) const;
    bool waitRecv (SOCKET s, int timeout = -1) const
    {
        bool in = true;
        bool out = false;
        return wait (s, in, out, timeout);
    }
    bool waitSend (SOCKET s, int timeout = -1) const
    {
        bool in = false;
        bool out = true;
        return wait (s, in, out, timeout);
    }
    void cancel () const;

private:
    EVENT  m_cancel;
};

#endif