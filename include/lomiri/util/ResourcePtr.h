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

#ifndef LOMIRI_UTIL_RESOURCEPTR_H
#define LOMIRI_UTIL_RESOURCEPTR_H

#include <mutex>
#include <type_traits>

namespace lomiri
{

namespace util
{

namespace
{

// Simple helper class so we can adopt a lock without inconvenient syntax.

template<typename T>
class LockAdopter
{
public:
    LockAdopter(T& mutex) noexcept
        : m_(mutex, std::adopt_lock)
    {
        assert(!mutex.try_lock()); // Mutex must be locked to be adoptable.
    }

private:
    std::unique_lock<T> m_;
};

} // namespace

/**
\brief Class to guarantee deallocation of arbitrary resources.

ResourcePtr is a generalized resource pointer that guarantees deallocation.
It is intended for arbitrary pairs of allocate/deallocate functions, such
as <code>XCreateDisplay</code>/<code>XDestroyDisplay</code>.

The resource managed by this class must be default-constructible, copy-constructible, and assignable.

ResourcePtr essentially does what <code>std::unique_ptr</code> does, but it works with opaque types
and resource allocation functions that do not return a pointer type, such as <code>open()</code>.

ResourcePtr is thread-safe.

\note Do not use reset() to set the resource to the "no resource allocated" state.
      Instead, call dealloc() to do this. ResourcePtr has no idea
      what a "not allocated" resource value looks like and therefore cannot test
      for it. If you use reset() to install a "no resource allocated" value for
      for the resource, the deleter will eventually be called with this value
      as its argument. Whether this is benign or not depends on the deleter. For
      example, <code>XFree()</code> must not be called with a <code>nullptr</code> argument.

\note Do not call get() or release() if no resource is currently allocated.
      Doing so throws <code>std::logic_error</code>.

Here is an example that shows how to use this for a <code>glXCreateContext</code>/<code>GLXDestroyContext</code> pair.
The returned <code>GLXContext</code> is a pointer to an opaque type; <code>std::unique_ptr</code> cannot be used
for this, even with a custom deleter, because the signatures of the allocator and deallocator
do not match <code>unique_ptr</code>'s expectations.

~~~
ResourcePtr<GLXContext, std::function<void(GLXContext)>> context =
  std::bind(&glXDestroyContext, display_, std::placeholders::_1);
~~~

display_ is declared as

~~~
  Display* display_;
~~~

in this case.

The deleter for the resource can return any type (including int, such as returned by <code>XDestroyWindow()</code>),
and it must accept a single argument of the resource type (<code>GLXContext</code> in this example).

<code>glXDestroyContext()</code> expects the display pointer as the first argument so, for this example,
<code>std::bind</code> converts the binary <code>glXDestroyContext()</code> function into a
unary function suitable as the deleter.

Rather than mucking around with <code>std::bind</code>, it is often easier to use a lambda. For example:

~~~
ResourcePtr<GLXContext, std::function<void(GLXContext)>> context =
  [this](GLXContext c) { this->dealloc_GLXContext(c); };
~~~

This calls a member function <code>dealloc_GLXContext()</code> that, in turn calls <code>glXDestroyContext()</code>
and supplies the display parameter.

\note Even though you can use ResourcePtr to deallocate dynamic memory, doing so is discouraged.
Use <code>std::unique_ptr</code> instead, which is better suited to the task.
*/

// TODO: Discuss throwing deleters and requirements (copy constructible, etc.) on deleter.

template<typename R, typename D>
class ResourcePtr final
{
public:
    /** Deleted */
    ResourcePtr(ResourcePtr const &) = delete;
    /** Deleted */
    ResourcePtr& operator=(ResourcePtr const&) = delete;
    /**
    \typedef element_type
    The type of resource managed by this ResourcePtr.
    */
    typedef R element_type;

    /**
    \typedef deleter_type
    A function object or lvalue reference to a function or function object. The ResourcePtr
    calls this to deallocate the resource.
    */
    typedef D deleter_type;

    ResourcePtr();
    explicit ResourcePtr(D d);
    ResourcePtr(R r, D d);
    ResourcePtr(ResourcePtr&& r);
    ResourcePtr& operator=(ResourcePtr&& r);
    ~ResourcePtr() noexcept;

    void swap(ResourcePtr& other);

    void reset(R r);
    R release();
    void dealloc();

    R get() const;
    bool has_resource() const noexcept;
    explicit operator bool() const noexcept;
    D& get_deleter() noexcept;
    D const& get_deleter() const noexcept;

    bool operator==(ResourcePtr const& rhs) const;

    bool operator!=(ResourcePtr const& rhs) const;

    bool operator<(ResourcePtr const& rhs) const;

    bool operator<=(ResourcePtr const& rhs) const;

    bool operator>(ResourcePtr const& rhs) const;

    bool operator>=(ResourcePtr const& rhs) const;

private:
    R resource_;                   // The managed resource.
    D delete_;                     // The deleter to call.
    bool initialized_;             // True while we have a resource assigned.
    mutable std::mutex m_;         // Protects this instance.

    typedef std::lock_guard<decltype(m_)>  AutoLock;
    typedef LockAdopter<decltype(m_)>      AdoptLock;
};

template<typename R, typename D>
ResourcePtr<R, D>::ResourcePtr()
    : initialized_(false)
{
    static_assert(!std::is_pointer<deleter_type>::value,
            "constructed with null function pointer deleter");
}

/**
Constructs a ResourcePtr with the specified deleter. No resource is held, so a call to has_resource()
after constructing a ResourcePtr this way returns <code>false</code>.
*/

template<typename R, typename D>
ResourcePtr<R, D>::ResourcePtr(D d)
    : delete_(d), initialized_(false)
{
}

/**
Constructs a ResourcePtr with the specified resource and deleter. has_resource() returns <code>true</code> after
calling this constructor.

\note It is legal to pass a resource that represents the "not allocated" state. For example, the
      following code passes the value <code>-1</code> to <code>close()</code> if the call to <code>open()</code> fails:

~~~
ResourcePtr<int, decltype(&::close)> fd(::open("/somefile", O_RDONLY), ::close);
~~~
      When the ResourcePtr goes out of scope, this results in a call to <code>close(-1)</code>. In this case,
      the call with an invalid file descriptor is harmless (but causes noise from diagnostic tools, such as
      <code>valgrind</code>). However, depending on the specific deleter, passing an invalid value to the deleter may
      have more serious consequences.

      To avoid the problem, you can delay initialization of the ResourcePtr until you know that the
      resource was successfully allocated, for example:
~~~
      int tmp_fd = ::open(filename.c_str(), O_RDONLY);
      if (tmp_fd == -1)
      {
          throw FileException(filename.c_str());
      }
      util::ResourcePtr<int, decltype(&::close)> fd(tmp_fd, ::close(fd));
~~~
      Alternatively, you can use a deleter function that tests the resource value
      for validity and avoids calling the deleter with an invalid value:
~~~
      util::ResourcePtr<int, std::function<void(int)>> fd(
          ::open(filename.c_str(), O_RDONLY),
          [](int fd) { if (fd != -1) ::close(fd); }
      );
~~~
      Note that, with the second approach, a call to get() will succeed and return -1 rather than throwing an
      exception, so the first approach is the recommended one.
*/

template<typename R, typename D>
ResourcePtr<R, D>::ResourcePtr(R r, D d)
    : resource_(r), delete_(d), initialized_(true)
{
}

/**
Constructs a ResourcePtr by transferring ownership from <code>r</code> to <code>this</code>.

If the resource's move or copy constructor throws, the exception is propagated to the caller.
The strong exception guarantee is preserved if it is provided by the resource.
*/
// TODO: Mark as nothrow if the resource has a nothrow move constructor or nothrow copy constructor

template<typename R, typename D>
ResourcePtr<R, D>::ResourcePtr(ResourcePtr<R, D>&& r)
    : resource_(std::move(r.resource_)), delete_(r.delete_), initialized_(r.initialized_)
{
    r.initialized_ = false; // Stop r from deleting its resource, if it held any. No need to lock: r is a temporary.
}

/**
Assigns the resource held by <code>r</code>, transferring ownership. After the transfer,
<code>r.has_resource()</code> returns <code>false</code>, and <code>this.has_resource()</code> returns
the value of <code>r.has_resource()</code> prior to the assignment.
*/
// TODO: document exception safety behavior

template<typename R, typename D>
ResourcePtr<R, D>& ResourcePtr<R, D>::operator=(ResourcePtr&& r)
{
    AutoLock lock(m_);

    if (initialized_)                   // If we hold a resource, deallocate it first.
    {
        initialized_ = false;           // If the deleter throws, we will not try it again for the same resource.
        delete_(resource_);             // Delete our own resource.
    }

    // r is a temporary, so we don't need to lock it.

    resource_ = std::move(r.resource_);
    initialized_ = r.initialized_;
    r.initialized_ = false;             // Stop r from deleting its resource, if it held any.
    delete_ = r.delete_;

    return *this;
}

/**
Destroys the ResourcePtr. If a resource is held, it calls the deleter for the current resource (if any).
*/

template<typename R, typename D>
ResourcePtr<R, D>::~ResourcePtr() noexcept
{
    try
    {
        dealloc();
    }
    catch (...)
    {
    }
}

/**
Swaps the resource and deleter of <code>this</code> with the resource and deleter of <code>other</code>
using argument dependent lookup (ADL).

If the underlying swap throws an exception, that exception is propagated to the caller, and the resource
held by the ResourcePtr is unchanged.
*/
// TODO Split this into throw and no-throw versions depending on the underlying swap?

template<typename R, typename D>
void ResourcePtr<R, D>::swap(ResourcePtr& other)
{
    if (this == &other)   // This is necessary to avoid deadlock for self-swap
    {
        return;
    }

    std::lock(m_, other.m_);
    AdoptLock left(m_);
    AdoptLock right(other.m_);

    using std::swap; // Enable ADL
    swap(resource_, other.resource_);
    swap(delete_, other.delete_);
    swap(initialized_, other.initialized_);
}

// The non-member swap() must be in the same namespace as ResourcePtr, so it will work with ADL. And, once it is
// defined here, there is no point in adding a specialization to namespace std any longer, because ADL
// will find it here anyway.

/**
Swaps the resource and deleter of <code>lhs</code> with the resource and deleter of <code>rhs</code>
by calling <code>lhs.swap(rhs)</code>.

If the underlying swap throws an exception, that exception is propagated to the caller, and the resource
held by the ResourcePtr is unchanged.
*/
// TODO Split this into throw and no-throw versions depending on the underlying swap?

template<typename R, typename D>
void swap(lomiri::util::ResourcePtr<R, D>& lhs, lomiri::util::ResourcePtr<R, D>& rhs)
{
    lhs.swap(rhs);
}

/**
Assigns a new resource to <code>this</code>, first deallocating the current resource (if any).

If the deleter for the current resource throws an exception, the exception is propagated to the caller. In this
case, the transfer of <code>r</code> to <code>this</code> is still carried out so, after the call to reset(),
<code>this</code> manages <code>r</code>, whether the deleter throws or not. (If the deleter <i>does</i> throw,
no attempt is made to call the deleter again for the same resource.)
*/

template<typename R, typename D>
void ResourcePtr<R, D>::reset(R r)
{
    AutoLock lock(m_);

    bool has_old = initialized_;
    R old_resource;

    if (has_old)
    {
        old_resource = resource_;
    }
    resource_ = r;
    initialized_ = true;    // If the deleter throws, we still satisfy the postcondition: resource_ == r.
    if (has_old)
    {
        delete_(old_resource);
    }
}

/**
Releases ownership of the current resource without calling the deleter.
\return The current resource.
\throw std::logic_error if has_resource() is false.
*/

template<typename R, typename D>
inline
R ResourcePtr<R, D>::release()
{
    AutoLock lock(m_);

    if (!initialized_)
    {
        throw std::logic_error("release() called on ResourcePtr without resource");
    }
    initialized_ = false;
    return resource_;
}

/**
Calls the deleter for the current resource.

If the deleter throws, the resource is considered in the "not allocated" state,
that is, no attempt is made to call the deleter again for this resource.
*/

template<typename R, typename D>
void ResourcePtr<R, D>::dealloc()
{
    AutoLock lock(m_);

    if (!initialized_)
    {
        return;
    }
    initialized_ = false;   // If the deleter throws, we will not try it again for the same resource.
    delete_(resource_);
}

/**
Returns the current resource. If no resource is currently held, get() throws <code>std::logic_error</code>.
\return The current resource (if any).

If the resource's copy constructor throws an exception, that exception is propagated to the caller.

\throw std::logic_error if has_resource() is false.
*/

template<typename R, typename D>
inline
R ResourcePtr<R, D>::get() const
{
    AutoLock lock(m_);

    if (!initialized_)
    {
        throw std::logic_error("get() called on ResourcePtr without resource");
    }
    return resource_;
}

/**
\return <code>true</code> if <code>this</code> currently manages a resource; <code>false</code>, otherwise.
*/

template<typename R, typename D>
inline
bool ResourcePtr<R, D>::has_resource() const noexcept
{
    AutoLock lock(m_);
    return initialized_;
}

/**
Synonym for has_resource().
*/

template<typename R, typename D>
inline
ResourcePtr<R, D>::operator bool() const noexcept
{
    return has_resource();
}

/**
\return The deleter for the resource.
*/

template<typename R, typename D>
inline
D& ResourcePtr<R, D>::get_deleter() noexcept
{
    AutoLock lock(m_);
    return delete_;
}

/**
\return The deleter for the resource.
*/

template<typename R, typename D>
inline
D const& ResourcePtr<R, D>::get_deleter() const noexcept
{
    AutoLock lock(m_);
    return delete_;
}

/**
\brief Compares two instances for equality.

Two instances that do not hold a resource are equal. An instance that does not hold a resource is not equal
to any instance that holds a resource.

If the underlying <code>operator==</code> throws an exception, that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator==</code>.
*/

template<typename R, typename D>
bool ResourcePtr<R, D>::operator==(ResourcePtr<R, D> const& rhs) const
{
    if (this == &rhs)   // This is necessary to avoid deadlock for self-comparison
    {
        return true;
    }

    std::lock(m_, rhs.m_);
    AdoptLock left(m_);
    AdoptLock right(rhs.m_);

    if (!initialized_)
    {
        return !rhs.initialized_;   // Equal if both are not initialized
    }
    else if (!rhs.initialized_)
    {
        return false;               // Not equal if lhs initialized, but rhs not initialized
    }
    else
    {
        return resource_ == rhs.resource_;
    }
}

/**
\brief Compares two instances for inequality.

If the underlying <code>operator==</code> throws an exception, that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator==</code>.
*/

template<typename R, typename D>
inline
bool ResourcePtr<R, D>::operator!=(ResourcePtr<R, D> const& rhs) const
{
    return !(*this == rhs);
}

/**
\brief Returns <code>true</code> if <code>this</code> is less than <code>rhs</code>.

An instance that does not hold a resource is less than any instance that holds a resource.

If the underlying <code>operator\<</code> throws an exception, that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator\<</code>.
*/

template<typename R, typename D>
bool ResourcePtr<R, D>::operator<(ResourcePtr<R, D> const& rhs) const
{
    if (this == &rhs)   // This is necessary to avoid deadlock for self-comparison
    {
        return false;
    }

    std::lock(m_, rhs.m_);
    AdoptLock left(m_);
    AdoptLock right(rhs.m_);

    if (!initialized_)
    {
        return rhs.initialized_;    // Not initialized is less than initialized
    }
    else if (!rhs.initialized_)     // Initialized is not less than not initialized
    {
        return false;
    }
    else
    {
        return resource_ < rhs.resource_;
    }
}

/**
\brief Returns <code>true</code> if <code>this</code> is less than or equal to <code>rhs</code>.

An instance that does not hold a resource is less than any instance that holds a resource.
Two instances that do not hold a resource are equal.

If the underlying <code>operator\<</code> or <code>operator==</code> throws an exception,
that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator\<</code>
and <code>operator==</code>.
*/

template<typename R, typename D>
bool ResourcePtr<R, D>::operator<=(ResourcePtr<R, D> const& rhs) const
{
    if (this == &rhs)   // This is necessary to avoid deadlock for self-comparison
    {
        return true;
    }

    // We can't just write:
    //
    // return *this < rhs || *this == rhs;
    //
    // because that creates a race condition: the locks would be released and
    // re-aquired in between the two comparisons.

    std::lock(m_, rhs.m_);
    AdoptLock left(m_);
    AdoptLock right(rhs.m_);

    return resource_ < rhs.resource_ || resource_ == rhs.resource_;
}

/**
\brief Returns <code>true</code> if <code>this</code> is greater than <code>rhs</code>.

An instance that holds a resource is greater than any instance that does not hold a resource.

If the underlying <code>operator\<</code> or <code>operator==</code> throws an exception,
that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator\<</code>
and <code>operator==</code>.
*/

template<typename R, typename D>
inline
bool ResourcePtr<R, D>::operator>(ResourcePtr<R, D> const& rhs) const
{
    return !(*this <= rhs);
}

/**
\brief Returns <code>true</code> if <code>this</code> is greater than or equal to <code>rhs</code>.

An instance that holds a resource is greater than any instance that does not hold a resource.
Two instances that do not hold a resource are equal.

If the underlying <code>operator\<</code> throws an exception, that exception is propagated to the caller.

\note This operator is available only if the underlying resource provides <code>operator\<</code>.
*/

template<typename R, typename D>
inline
bool ResourcePtr<R, D>::operator>=(ResourcePtr<R, D> const& rhs) const
{
    return !(*this < rhs);
}

} // namespace util

} // namespace lomiri

// Specializations in namespace std, so we play nicely with STL and metaprogramming.

namespace std
{

/**
\brief Function object for equality comparison.
*/

template<typename R, typename D>
struct equal_to<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator==</code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs == rhs;
    }
};

/**
\brief Function object for inequality comparison.
*/

template<typename R, typename D>
struct not_equal_to<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator!=</code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs != rhs;
    }
};

/**
\brief Function object for less than comparison.
*/

template<typename R, typename D>
struct less<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator\<</code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs < rhs;
    }
};

/**
\brief Function object for less than or equal comparison.
*/

template<typename R, typename D>
struct less_equal<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator\<=</code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs <= rhs;
    }
};

/**
\brief Function object for greater than comparison.
*/

template<typename R, typename D>
struct greater<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator\></code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs > rhs;
    }
};

/**
\brief Function object for less than or equal comparison.
*/

template<typename R, typename D>
struct greater_equal<lomiri::util::ResourcePtr<R, D>>
{
    /**
    Invokes <code>operator\>=</code> on <code>lhs</code>.
    */
    bool operator()(lomiri::util::ResourcePtr<R, D> const& lhs, lomiri::util::ResourcePtr<R, D> const& rhs) const
    {
        return lhs >= rhs;
    }
};

// TODO: provide hash if std::hash<R> exists.

} // namespace std

#endif
