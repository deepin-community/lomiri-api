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
 * Authored by: Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *              Pete Woods <pete.woods@canonical.com>
 */

#ifndef LOMIRI_UTIL_GOBJECTMEMORY_H
#define LOMIRI_UTIL_GOBJECTMEMORY_H

#include <memory>
#include <stdexcept>
#include <glib-object.h>

#include <lomiri/util/ResourcePtr.h>

namespace lomiri
{

namespace util
{

namespace
{

inline static void check_floating_gobject(gpointer t)
{
    if (G_IS_OBJECT(t) && g_object_is_floating(G_OBJECT(t)))
    {
        throw std::invalid_argument("cannot manage floating GObject reference - call g_object_ref_sink(o) first");
    }
}

}

/**
 \brief Used by the make_gobject, unique_gobject and share_gobject as the deleter.

 Useful if for some reason the normal helper methods are not suitable for your needs.

 Example:
 \code{.cpp}
 GObjectDeleter d;
 auto shared = shared_ptr<FooBar>(foo_bar_new("name"), d);
 auto unique = unique_ptr<FooBar, GObjectDeleter>(foo_bar_new("name"), d);
 \endcode
 */
struct GObjectDeleter
{
    void operator()(gpointer ptr) noexcept
    {
        if (G_IS_OBJECT(ptr))
        {
            g_object_unref(ptr);
        }
    }
};

template<typename T> using GObjectSPtr = std::shared_ptr<T>;
template<typename T> using GObjectUPtr = std::unique_ptr<T, GObjectDeleter>;

namespace internal
{

template<typename SP>
class GObjectAssigner
{
public:
    typedef typename SP::element_type ElementType;

    GObjectAssigner(SP& smart_ptr) noexcept:
            smart_ptr_(smart_ptr)
    {
    }

    GObjectAssigner(const GObjectAssigner& other) = delete;

    GObjectAssigner(GObjectAssigner&& other) noexcept:
            ptr_(other.ptr_), smart_ptr_(other.smart_ptr_)
    {
        other.ptr_ = nullptr;
    }

    ~GObjectAssigner() noexcept
    {
        smart_ptr_ = SP(ptr_, GObjectDeleter());
    }

    GObjectAssigner& operator=(const GObjectAssigner& other) = delete;

    operator ElementType**() noexcept
    {
        return &ptr_;
    }

private:
    ElementType* ptr_ = nullptr;

    SP& smart_ptr_;
};

template <typename T>
struct GObjectSignalUnsubscriber
{
    void operator()(gulong handle) noexcept
    {
        if (handle != 0 && G_IS_OBJECT(obj_.get()))
        {
            g_signal_handler_disconnect(obj_.get(), handle);
        }
    }

    GObjectSPtr<T> obj_;
};

}

/**
 \brief Helper method to wrap a unique_ptr around an existing GObject.

 Useful if the GObject class you are constructing already has a
 dedicated factory from the C library it comes from, and you
 intend to have a unique instance of it.

 Example:
 \code{.cpp}
 auto obj = unique_gobject(foo_bar_new("name"));
 \endcode
 */
template<typename T>
inline GObjectUPtr<T> unique_gobject(T* ptr)
{
    check_floating_gobject(ptr);
    GObjectDeleter d;
    return GObjectUPtr<T>(ptr, d);
}

/**
 \brief Helper method to wrap a shared_ptr around an existing GObject.

 Useful if the GObject class you are constructing already has a
 dedicated factory from the C library it comes from, and you
 intend to share it.

 Example:
 \code{.cpp}
 auto obj = share_gobject(foo_bar_new("name"));
 \endcode
 */
template<typename T>
inline GObjectSPtr<T> share_gobject(T* ptr)
{
    check_floating_gobject(ptr);
    GObjectDeleter d;
    return GObjectSPtr<T>(ptr, d);
}

/**
 \brief Helper method to construct a gobj_ptr-wrapped GObject class.

 Uses the same signature as the g_object_new() method.

 Example:
 \code{.cpp}
 auto obj = make_gobject<FooBar>(FOO_TYPE_BAR, "name", "banana", nullptr);
 \endcode
 */
template<typename T, typename ... Args>
inline GObjectUPtr<T> make_gobject(GType object_type, const gchar *first_property_name, Args&&... args) noexcept
{
    gpointer ptr = g_object_new(object_type, first_property_name, std::forward<Args>(args)...);
    if (G_IS_OBJECT(ptr) && g_object_is_floating(ptr))
    {
        g_object_ref_sink(ptr);
    }
    return unique_gobject(G_TYPE_CHECK_INSTANCE_CAST(ptr, object_type, T));
}

/**
 \brief Helper method to take ownership of GObjects assigned from a reference.

 Example:
 \code{.cpp}
 GObjectUPtr<FooBar> o;
 method_that_assigns_a_foobar(assign_gobject(o));
 \endcode
 */
template<typename SP>
inline internal::GObjectAssigner<SP> assign_gobject(SP& smart_ptr) noexcept
{
    return internal::GObjectAssigner<SP>(smart_ptr);
}

template<typename T>
using GObjectSignalConnection = ResourcePtr<gulong, internal::GObjectSignalUnsubscriber<T>>;

/**
 \brief Simple wrapper to manage the lifecycle of GObject signal connections.

 When 'nameConnection_' goes out of scope or is dealloc'ed, the source will be removed:
 \code{.cpp}
 GObjectSignalConnection<FooBar> nameConnection_;
 nameConnection_ = gobject_signal_connection(g_signal_connect(o.get(), "notify::name", G_CALLBACK(on_notify_name), this), o);
 \endcode
 */
template <typename T>
inline GObjectSignalConnection<T> gobject_signal_connection(gulong id, const GObjectSPtr<T>& obj)
{
    return GObjectSignalConnection<T>(id, internal::GObjectSignalUnsubscriber<T>{obj});
}

}  // namespace until

}  // namespace lomiri

#endif
