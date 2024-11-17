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

#include <memory>

#include "receiver.hpp"
#include "tcpsocket.hpp"
#include "rawsocket.hpp"
#include "console.hpp"
#include "tunnel.hpp"


Receiver::Receiver (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket, std::binary_semaphore* finished)
: m_thread (&Receiver::threadFunc, this, mtu, inputSocket, outputSocket, finished)
{

}

Receiver::~Receiver ()
{
    m_thread.join ();
}

void Receiver::threadFunc (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket, std::binary_semaphore* finished)
{
    Console::PrintDebug ("Receiver started\n");
    try
    {
        const size_t headerLen = sizeof (TunnelHeader);
        std::unique_ptr<uint8_t[]> data (new uint8_t[headerLen + mtu]);
        ssize_t payloadLen;

        while ((payloadLen = inputSocket->recv (data.get() + headerLen, mtu)) > 0)
        {
            outputSocket->send (
                TunnelHeader::packet(data.get(), (uint32_t) payloadLen),
                headerLen + payloadLen);
        }
    }
    catch(const SocketException& e)
    {
        Console::PrintError ("%s\n", e.what());
    }

    Console::PrintDebug ("Receiver terminated\n");

    if (finished)
        finished->release ();
}