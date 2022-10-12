/*
 * Copyright (C) 2020 UBports foundation
 * Author(s): Marius Gripsgard <marius@ubports.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <lomiri/SymbolExport.h>

#include <string>

namespace lomiri
{

namespace util
{
    LOMIRI_API std::string dbus_sanitize_str(const std::string& str);
    LOMIRI_API std::string dbus_sanitized_path(const std::string& base, const std::string& str);
    LOMIRI_API inline std::string dbus_sanitized_path(const std::string& str)
    {
         return dbus_sanitized_path("/", str);
    }

} // namespace util

} // namespace lomiri
