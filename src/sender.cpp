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
#include <cstring>
#include <stdexcept>

#include "sender.hpp"
#include "tcpsocket.hpp"
#include "rawsocket.hpp"
#include "console.hpp"
#include "tunnel.hpp"


static inline size_t ptrdiff_to_len (const void* p1, const void* p2)
{
    BUG_ON (p2 > p1);
    return (size_t)((uint8_t*)p1 - (uint8_t*)p2);
}


Sender::Sender (unsigned mtu, const RawSocket* outputSocket, const TcpSocket* inputSocket, std::binary_semaphore* finished)
: m_thread (&Sender::threadFunc, this, mtu, outputSocket, inputSocket, finished)
{

}

Sender::~Sender ()
{
    m_thread.join ();
}

void Sender::threadFunc (unsigned mtu, const RawSocket* outputSocket, const TcpSocket* inputSocket, std::binary_semaphore* finished)
{
    Console::PrintDebug ("Sender started\n");

    try
    {
        const size_t headerLen = sizeof (TunnelHeader);
        const size_t bufSize = (headerLen + mtu) * 10;
        std::unique_ptr<uint8_t[]> data (new uint8_t[bufSize]);

        uint8_t* buf = data.get();
        uint8_t* in = buf;

        while (1)
        {
            const TunnelHeader* pHeader = (TunnelHeader*)buf;

            // receive until we have a full header
            while (ptrdiff_to_len (in, buf) < headerLen)
                in += inputSocket->recv (in, bufSize - ptrdiff_to_len (in, buf));

            // check the payload size
            uint32_t payloadLen = pHeader->getLength();
            if (payloadLen > mtu)
                throw std::length_error ("Length exceeds MTU of interface");

            // receive until we have a full payload
            while (ptrdiff_to_len (in, buf) < payloadLen + headerLen)
                in += inputSocket->recv (in, bufSize - ptrdiff_to_len (in, buf));

            // process the received stuff (might contain multiple packets)
            do
            {
                if (pHeader->isPacket())
                    outputSocket->send (pHeader->payload(), pHeader->getLength());
                pHeader = pHeader->next ();

            } while ((uint8_t*)pHeader + headerLen < in && pHeader->payload() + pHeader->getLength() < in);

            BUG_ON (in < (uint8_t*)pHeader);

            if ((uint8_t*)pHeader == in)
            {
                // no fragments left, reset pointer
                in = buf;
            }
            else
            {
                // copy fragments to to start of buffer
                // processing will be finished on next loop iteration
                std::memmove (buf, pHeader, in - (uint8_t*)pHeader);
                in = buf + (in - (uint8_t*)pHeader);
            }
        }

        /*
        while ()
        {
            // hier sind wir immer am Puffer Anfang, es kann aber schon ein Fragment vorhanden sein
            // --> header passt immer rein
            // --> header könnte schon teilweise oder vollständig vorhanden sein
            // --> auf offset 0 liegt immer ein Header
            // --> payload passt immer rein
            // --> payload könnte schon teilweise vorhanden sein

            empfangen so lange bis mindestens ein packet vollständig ist ()
                so lange empfangen, bis inIndex >= headerLen
                Header auswerten
                so lange empfangen, bis mindestens eine payload vollständig ist
                inIndex in allen Schritten aktualisieren
            alle empfangenen Packete versenden ()
                outIndex setzen
            puffer auf Anfang setzen ()
                if (fragmente vorhanden)
                    fragmente auf Anfang kopieren
                    in und out Index entsprechend setzen
                else
                    inIndex = outIndex = 0
        }
        */
    }
    catch(const std::exception& e)
    {
        Console::PrintError ("%s\n", e.what());
    }
    Console::PrintDebug ("Sender terminated\n");

    if (finished)
        finished->release ();
}