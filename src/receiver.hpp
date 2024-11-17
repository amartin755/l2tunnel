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


#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <thread>

class RawSocket;
class TcpSocket;

class Receiver
{
public:
    Receiver (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket, std::binary_semaphore* finished = nullptr);
    ~Receiver ();
    void join ()
    {
        m_thread.join ();
    }

    void threadFunc (unsigned mtu, const RawSocket* inputSocket, const TcpSocket* outputSocket, std::binary_semaphore* finished);

private:
    std::thread m_thread;
};



#endif