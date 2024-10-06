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

#include "sender.hpp"
#include "tcpsocket.hpp"
#include "rawsocket.hpp"
#include "console.hpp"

Sender::Sender (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket)
: m_thread (&Sender::threadFunc, this, mtu, inputSocket, outputSocket)
{

}

Sender::~Sender ()
{
    m_thread.join ();
}

void Sender::threadFunc (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket)
{
    try
    {
        uint8_t buf[10*1024];
        ssize_t index = 0;

        do
        {
            index += outputSocket->recv (&buf[index], sizeof (buf) - index);

        } while (1);
    }
    catch(const SocketException& e)
    {
        Console::PrintError ("%s", e.what());
    }
}