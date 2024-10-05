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
#include <iostream>
#include "tcpsocket.hpp"
#include "rawsocket.hpp"

int main (int argc, char** argv)
{
    try
    {
#if 0
        RawSocket s = RawSocket::open ("lo");
            char buf[1024];
            ssize_t len;
            s.send ("HAAAAAAAAAAAALLLLLOOOOOOOOO", sizeof ("HAAAAAAAAAAAALLLLLOOOOOOOOO"));
            while ((len = s.recv (buf, sizeof (buf))) > 0)
            {
                buf[len] = '\0';
                std::cout << len << std::endl;
            }
#else
        if (argc > 1)
        {
            TcpSocket connection = TcpSocket::connect (argv[1], 40000);
            char buf[1024];
            ssize_t len;
            while ((len = connection.recv (buf, sizeof (buf))) > 0)
            {
                buf[len] = '\0';
                std::cout << buf;
            }
        }
        else
        {
            TcpSocket server = TcpSocket::listen (40000, 0);

            std::string addr;
            uint16_t port;

            TcpSocket connection = server.accept (addr, port);
            std::cout << addr << ":" << port << std::endl;
            
            char buf[1024];
            ssize_t len;
            while ((len = connection.recv (buf, sizeof (buf))) > 0)
            {
                buf[len] = '\0';
                std::cout << buf;
            }
        }
#endif
    }
    catch(const SocketException& e)
    {
        std::cerr << e.what() << '\n';
    }
    

    return 0;
}
