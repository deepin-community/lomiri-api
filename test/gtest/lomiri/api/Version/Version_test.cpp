/*
 * Copyright (C) 2013 Canonical Ltd
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
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <lomiri/api/Version.h>

#include <gtest/gtest.h>

using namespace lomiri::api;

TEST(Version, basic)
{
    EXPECT_EQ(major_version(), LOMIRI_API_VERSION_MAJOR);
    EXPECT_EQ(minor_version(), LOMIRI_API_VERSION_MINOR);
    EXPECT_EQ(micro_version(), LOMIRI_API_VERSION_MICRO);
    EXPECT_STREQ(str(), LOMIRI_API_VERSION_STRING);
}
