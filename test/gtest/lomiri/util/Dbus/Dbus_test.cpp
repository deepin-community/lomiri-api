/*
 * Copyright (C) 2020 UBports foundation
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

#include <gtest/gtest.h>

using namespace lomiri::util;

namespace {

TEST(Utilities, testBasicDbusStr) {
    EXPECT_EQ("bar", dbus_sanitize_str("bar"));
    EXPECT_EQ("foo_5fbar", dbus_sanitize_str("foo_bar"));
    EXPECT_EQ("foo_2ebar", dbus_sanitize_str("foo.bar"));
    EXPECT_EQ("foo_20bar", dbus_sanitize_str("foo bar"));
    EXPECT_EQ("foo_2fbar_2ebaz", dbus_sanitize_str("foo/bar.baz"));
}

TEST(Utilities, testBasicDbusPath) {
    EXPECT_EQ("/bar", dbus_sanitized_path("bar"));
    EXPECT_EQ("/foo_5fbar", dbus_sanitized_path("foo_bar"));
    EXPECT_EQ("/foo_2ebar", dbus_sanitized_path("foo.bar"));
    EXPECT_EQ("/foo_20bar", dbus_sanitized_path("foo bar"));
    EXPECT_EQ("/foo_2fbar_2ebaz", dbus_sanitized_path("foo/bar.baz"));
}

TEST(Utilities, testBasicDbusPathBase) {
    EXPECT_EQ("/com/blah/bar", dbus_sanitized_path("/com/blah/", "bar"));
    EXPECT_EQ("/com/blah/foo_5fbar", dbus_sanitized_path("/com/blah/", "foo_bar"));
    EXPECT_EQ("/com/blah/foo_2ebar", dbus_sanitized_path("/com/blah/", "foo.bar"));
    EXPECT_EQ("/com/blah/foo_20bar", dbus_sanitized_path("/com/blah/", "foo bar"));
    EXPECT_EQ("/com/blah/foo_2fbar_2ebaz", dbus_sanitized_path("/com/blah/", "foo/bar.baz"));
}

TEST(Utilities, testBasicDbusPathBaseMissingSlash) {
    EXPECT_EQ("/com/blah/bar", dbus_sanitized_path("com/blah", "bar"));
    EXPECT_EQ("/com/blah/bar", dbus_sanitized_path("com/blah/", "bar"));
    EXPECT_EQ("/com/blah/bar", dbus_sanitized_path("/com/blah", "bar"));

    EXPECT_EQ("/com/blah/foo_5fbar", dbus_sanitized_path("com/blah", "foo_bar"));
    EXPECT_EQ("/com/blah/foo_2ebar", dbus_sanitized_path("com/blah", "foo.bar"));
    EXPECT_EQ("/com/blah/foo_20bar", dbus_sanitized_path("com/blah", "foo bar"));
    EXPECT_EQ("/com/blah/foo_2fbar_2ebaz", dbus_sanitized_path("com/blah", "foo/bar.baz"));
}

} // namespace
