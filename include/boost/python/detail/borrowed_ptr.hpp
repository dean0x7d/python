#ifndef BORROWED_PTR_DWA20020601_HPP
# define BORROWED_PTR_DWA20020601_HPP
// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

# include <boost/python/tag.hpp>

namespace boost { namespace python { namespace detail {

template<class T> 
class borrowed { 
    using type = T;
};

template<typename T>
struct is_borrowed_ptr {
    static constexpr bool value = false;
};

template<typename T>
struct is_borrowed_ptr<borrowed<T>*> {
    static constexpr bool value = true;
};

template<typename T>
struct is_borrowed_ptr<borrowed<T> const*> {
    static constexpr bool value = true;
};

template<typename T>
struct is_borrowed_ptr<borrowed<T> volatile*> {
    static constexpr bool value = true;
};

template<typename T>
struct is_borrowed_ptr<borrowed<T> const volatile*> {
    static constexpr bool value = true;
};

}

template <class T>
inline T* get_managed_object(detail::borrowed<T> const volatile* p, tag_t)
{
    return (T*)p;
}

}} // namespace boost::python::detail

#endif // #ifndef BORROWED_PTR_DWA20020601_HPP
