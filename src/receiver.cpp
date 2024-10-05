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

#include "receiver.hpp"
#include "tcpsocket.hpp"
#include "rawsocket.hpp"
#include "console.hpp"

Receiver::Receiver (const RawSocket* inputSocket, const TcpSocket* outputSocket)
: m_thread (&Receiver::threadFunc, this, inputSocket, outputSocket)
{

}

Receiver::~Receiver ()
{
    m_thread.join ();
}

void Receiver::threadFunc (const RawSocket* inputSocket, const TcpSocket* outputSocket)
{
    try
    {
        struct {
            uint32_t type;
            uint32_t len;
            uint8_t  data[1024];
        }packet;

        while ((packet.len = inputSocket->recv (packet.data, sizeof (packet.data))) > 0)
        {
            packet.type = 0x12345678;
            outputSocket->send (&packet, packet.len + sizeof (packet.type) + sizeof (packet.len));
        }
    }
    catch(const SocketException& e)
    {
        Console::PrintError ("%s", e.what());
    }
}