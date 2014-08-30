// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef VOID_PTR_DWA200239_HPP
# define VOID_PTR_DWA200239_HPP

namespace boost { namespace python { namespace detail { 

template <class U>
inline U& void_ptr_to_reference(void const volatile* p, U&(*)())
{
    return *(U*)p;
}

template <class T>
inline void write_void_ptr(void const volatile* storage, void* ptr, T*)
{
    *(T**)storage = (T*)ptr;
}

// writes U(ptr) into the storage
template <class U>
inline void write_void_ptr_reference(void const volatile* storage, void* ptr, U&(*)())
{
    write_void_ptr(storage, ptr, U(nullptr));
}

}}} // namespace boost::python::detail

#endif // VOID_PTR_DWA200239_HPP
