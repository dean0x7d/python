// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef HANDLE_DWA200269_HPP
# define HANDLE_DWA200269_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/cast.hpp>
# include <boost/python/errors.hpp>
# include <boost/python/borrowed.hpp>
# include <boost/python/handle_fwd.hpp>
# include <boost/python/refcount.hpp>
# include <boost/python/detail/raw_pyobject.hpp>

namespace boost { namespace python { 

template <class T> struct null_ok;

template <class T>
inline null_ok<T>* allow_null(T* p)
{
    return (null_ok<T>*)p;
}

namespace detail
{
  template <class T>
  inline T* manage_ptr(detail::borrowed<null_ok<T> >* p, int)
  {
      return python::xincref((T*)p);
  }
  
  template <class T>
  inline T* manage_ptr(null_ok<detail::borrowed<T> >* p, int)
  {
      return python::xincref((T*)p);
  }
  
  template <class T>
  inline T* manage_ptr(detail::borrowed<T>* p, long)
  {
      return python::incref(expect_non_null((T*)p));
  }
  
  template <class T>
  inline T* manage_ptr(null_ok<T>* p, long)
  {
      return (T*)p;
  }
  
  template <class T>
  inline T* manage_ptr(T* p, ...)
  {
      return expect_non_null(p);
  }
}

template <class T>
class handle
{
    typedef T* (handle::* bool_type )() const;

 public: // types
    typedef T element_type;
    
 public: // member functions
    handle();
    ~handle();

    template <class Y>
    explicit handle(Y* p)
        : m_p(
            python::upcast<T>(
                detail::manage_ptr(p, 0)
                )
            )
    {
    }

    handle& operator=(handle const& r)
    {
        python::xdecref(m_p);
        m_p = python::xincref(r.m_p);
        return *this;
    }

    template<typename Y>
    handle& operator=(handle<Y> const & r) // never throws
    {
        python::xdecref(m_p);
        m_p = python::xincref(python::upcast<T>(r.get()));
        return *this;
    }

    template <typename Y>
    handle(handle<Y> const& r)
        : m_p(python::xincref(python::upcast<T>(r.get())))
    {
    }
    
    handle(handle const& r)
        : m_p(python::xincref(r.m_p))
    {
    }
    
    T* operator-> () const;
    T& operator* () const;
    T* get() const;
    T* release();
    void reset();
    
    operator bool_type() const // never throws
    {
        return m_p ? &handle<T>::get : 0;
    }
    bool operator! () const; // never throws

 public: // implementation details -- do not touch
    // Defining this in the class body suppresses a VC7 link failure
    inline handle(detail::borrowed_reference x)
        : m_p(
            python::incref(
                downcast<T>((PyObject*)x)
                ))
    {
    }
    
 private: // data members
    T* m_p;
};

typedef handle<PyTypeObject> type_handle;

//
// Compile-time introspection
//
template<typename T>
struct is_handle : std::false_type {};

template<typename T>
struct is_handle<handle<T>> : std::true_type {};

//
// implementations
//
template <class T>
inline handle<T>::handle()
    : m_p(0)
{
}

template <class T>
inline handle<T>::~handle()
{
    python::xdecref(m_p);
}

template <class T>
inline T* handle<T>::operator->() const
{
    return m_p;
}

template <class T>
inline T& handle<T>::operator*() const
{
    return *m_p;
}

template <class T>
inline T* handle<T>::get() const
{
    return m_p;
}
    
template <class T>
inline bool handle<T>::operator!() const
{
    return m_p == 0;
}

template <class T>
inline T* handle<T>::release()
{
    T* result = m_p;
    m_p = 0;
    return result;
}

template <class T>
inline void handle<T>::reset()
{
    python::xdecref(m_p);
    m_p = 0;
}

// Because get_managed_object must return a non-null PyObject*, we
// return Py_None if the handle is null.
template <class T>
inline PyObject* get_managed_object(handle<T> const& h)
{
    return h.get() ? python::upcast<PyObject>(h.get()) : Py_None;
}

}} // namespace boost::python


#endif // HANDLE_DWA200269_HPP
