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

#include "main.hpp"
#include "tcpsocket.hpp"
#include "rawsocket.hpp"
#include "receiver.hpp"


Application::Application(const char* name, const char* brief, const char* usage, const char* description, const char* version,
        const char* build, const char* buildDetails)
: cCmdlineApp (name, brief, usage, description, version, build, buildDetails)
{
    addCmdLineOption (false, 'i', "interface", "IFC",
            "Name of the network interface via which the packets are sent."
#if HAVE_WINDOWS
            "\n\t"
            "It can either be the AdapterName (GUID) like \"{3F4A136A-2ED5-4226-9CB2-7A511E93CD48}\", \n\t"
            "or the so-called FriendlyName, which is changeable by the user.\n\t"
            "For example \"WiFi\" or \"Local Area Connection 1\"."
#endif
            , &m_options.l2Interface);
    addCmdLineOption (true, 'l', "listen", "PORT",
            "Listen for incoming connections on PORT.", &m_options.serverPort);
    addCmdLineOption (true, '4', nullptr,
            "Force the use of IPv4 only.", &m_options.ipv4Only);
    addCmdLineOption (true, '6', nullptr,
            "Force the use of IPv6 only.", &m_options.ipv6Only);
}

Application::~Application ()
{

}

int Application::execute (const std::list<std::string>& args)
{
    bool isServer = !!m_options.serverPort;

    try
    {
        RawSocket s = RawSocket::open (m_options.l2Interface);

        if (isServer)
        {
            TcpSocket server = TcpSocket::listen (m_options.serverPort, 1);

            std::string addr;
            uint16_t port;

            TcpSocket connection = server.accept (addr, port);
            std::cout << addr << ":" << port << std::endl;

        }
        else
        {
            if (args.size() != 2)
            {
                Console::PrintError ("You must specify a host and port to connect to.\n");
                return -1;
            }
            int port = 0;
            try
            {
                port = std::stoi (args.back());
            }
            catch(...)
            {
            }
            if (port < 1 || port > 65535)
            {
                Console::PrintError ("Invalid port numer '%s'.\n", args.back().c_str());
                return -1;
            }

            TcpSocket connection = TcpSocket::connect (args.front(), port);
            Receiver receiverThread (&s, &connection);
            while (1)
            {
                sleep (1);
            }
        }
    }
    catch (const SocketException& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}


int main (int argc, char** argv)
{
    Application app (
            "l2tunnel",
            "A simple layer 2 Ethernet tunnel",
            "l2tun [OPTIONS] [hostname] [port]",
            "Homepage: <https://github.com/amartin755/l2tunnel>",
            APP_VERSION, BUILD_TIME,  GIT_BRANCH GIT_COMMIT BUILD_TYPE);
    return app.main (argc, argv);
}
