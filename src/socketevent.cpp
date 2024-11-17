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

#if HAVE_EVENTFD
#include <sys/eventfd.h>
#endif
#if HAVE_WINDOWS
#else
#include <unistd.h>
#include <poll.h>
#endif

#include "bug.hpp"
#include "socketevent.hpp"
#include "socketexception.hpp"


SocketEvent::SocketEvent () : m_cancel (INVALID_EVENT)
{
#if HAVE_EVENTFD
    m_cancel = eventfd (0, 0);
#endif
    if (m_cancel == INVALID_EVENT)
        throw SocketException();
}

SocketEvent::SocketEvent (SocketEvent&& obj)
{
    m_cancel = obj.m_cancel;
    obj.m_cancel = INVALID_EVENT;
}

SocketEvent::~SocketEvent ()
{
    if (m_cancel != INVALID_EVENT)
    {
#if HAVE_EVENTFD
        ::close (m_cancel);
#endif
        m_cancel = INVALID_EVENT;
    }
}

bool SocketEvent::wait (SOCKET s, bool &recv, bool &send, int timeout) const
{
    struct pollfd pollfd[] = {
        {
            m_cancel,
            POLLIN,
            0
        },
        {
            s,
            (short)((recv ? POLLIN : 0) | (send ? POLLOUT : 0)),
            0
        }
    };

    int ret = poll (pollfd, sizeof (pollfd) / sizeof (struct pollfd), timeout);
    if (ret < 0)
        throw SocketException();

    // timeout
    if (ret == 0)
    {
        BUG_ON (timeout < 0);
        recv = send = false;
        return false;
    }

    BUG_ON (pollfd[0].revents & (~POLLIN));

    // cancel request
    if (pollfd[0].revents & POLLIN)
    {
        throw SocketException ("");
    }

    // connection terminated
    if (pollfd[1].revents & (POLLERR | POLLHUP | POLLNVAL))
    {
        throw SocketException ("Connection reset by peer");
    }
    // data received
    recv = pollfd[1].revents & POLLIN;
    // ready for send
    send = pollfd[1].revents & POLLOUT;

    return recv || send;
}


void SocketEvent::cancel () const
{
#if HAVE_EVENTFD
    const uint64_t count = 1;
    if (write (m_cancel, &count, sizeof(count)) != sizeof (count))
        throw SocketException();
#endif
}
