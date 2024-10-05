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


#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#include <list>
#include <cstddef>
#include <csignal>

#include "cmdlineapp.hpp"

struct appOptions
{
    const char*  l2Interface;
    const char*  serverIP;
    int          serverPort;
    int          ipv4Only;
    int          ipv6Only;

    appOptions () :
        l2Interface (nullptr),
        serverIP (nullptr),
        serverPort (0),
        ipv4Only (0),
        ipv6Only (0)
    {
    }
};


class Application : public cCmdlineApp
{
public:
    Application (const char* name, const char* brief, const char* usage, const char* description, const char* version,
        const char* build, const char* buildDetails);
    virtual ~Application();

    int execute (const std::list<std::string>& args);

private:
    appOptions m_options;

};

#endif /* APPLICATION_HPP */
