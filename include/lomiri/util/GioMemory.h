/*
 * Copyright (C) 2013-2017 Canonical Ltd.
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
 * Authored by: Pete Woods <pete.woods@canonical.com>
 */

#ifndef LOMIRI_UTIL_GIOMEMORY_H
#define LOMIRI_UTIL_GIOMEMORY_H

#include <gio/gio.h>

#include <lomiri/util/GObjectMemory.h>

namespace lomiri
{

namespace util
{

namespace internal
{
struct GDBusSignalUnsubscriber
{
public:
    void operator()(guint handle) noexcept
    {
        if (handle != 0 && G_IS_OBJECT(bus_.get()))
        {
            g_dbus_connection_signal_unsubscribe(bus_.get(), handle);
        }
    }

    GObjectSPtr<GDBusConnection> bus_;
};

}

typedef ResourcePtr<guint, internal::GDBusSignalUnsubscriber> GDBusSignalConnection;

/**
 \brief Simple wrapper to manage the lifecycle of manual GDBus signal connections.

 When 'signalConnection_' goes out of scope or is dealloc'ed, the connection will be removed:
 \code{.cpp}
 GDBusSignalConnection signalConnection_;

 signalConnection_ = gdbus_signal_connection(
            g_dbus_connection_signal_subscribe(bus.get(), nullptr, "org.does.not.exist", nullptr, "/does/not/exist", nullptr, G_DBUS_SIGNAL_FLAGS_NONE, on_dbus_signal, this, nullptr), bus);
 \endcode
 */
inline GDBusSignalConnection gdbus_signal_connection(guint id, GObjectSPtr<GDBusConnection> bus) noexcept
{
    return GDBusSignalConnection(id, internal::GDBusSignalUnsubscriber{bus});
}

}  // namespace until

}  // namespace lomiri

#endif
