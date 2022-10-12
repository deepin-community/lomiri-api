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

#include "lomiri/util/Dbus.h"

namespace lomiri
{

namespace util
{

std::string dbus_sanitize_str(const std::string& str)
{
    std::string output;
    for (auto ch : str) {
        if (std::isalnum(ch, std::locale::classic())) {
            output.push_back(ch);
        } else {
            char buff[100];
            snprintf(buff, sizeof(buff), "_%02x", ch);
            output.append(buff);
        }
    }
    output.shrink_to_fit();
    return output;
}

std::string dbus_sanitized_path(const std::string& base, const std::string& path)
{
    std::string output;
    if (base.front() != '/')
        output.push_back('/');
    output += base;
    if (output.back() != '/')
        output.push_back('/');
    output += dbus_sanitize_str(path);
    output.shrink_to_fit();
    return output;
}

} // namespace util

} // namespace lomiri
