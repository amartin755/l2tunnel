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

#include <stdexcept>
#include <cstring>

#if HAVE_WINDOWS
#include <windows.h>
#else
#include <cerrno>
#endif

#include "socketexception.hpp"

SocketException::SocketException ()
{
    int error;
#if !HAVE_WINDOWS
    error = errno;
#else
    error = WSAGetLastError ();
#endif

    const char* ret = strerrordesc_np (error);
    if (ret)
        m_what = ret;
}

SocketException::SocketException (const std::string& what)
{
    m_what = what;
}

SocketException::SocketException (const char* what)
{
    m_what = what;
}

const char* SocketException::what() const noexcept
{
    return m_what.c_str();
}
