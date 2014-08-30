// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef UNWRAP_WRAPPER_DWA2004723_HPP
# define UNWRAP_WRAPPER_DWA2004723_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/python/detail/is_wrapper.hpp>

namespace boost { namespace python { namespace detail { 

template <class T, bool is_wrapper = false>
struct unwrap_wrapper_impl {
    using type = T;
};

template <class T>
struct unwrap_wrapper_impl<T, true> {
    using type = typename T::_wrapper_wrapped_type_;
};

template <class T>
using unwrap_wrapper_t = typename unwrap_wrapper_impl<T, is_wrapper<T>::value>::type;

template <class T>
unwrap_wrapper_t<T>* unwrap_wrapper(T*) {
    return nullptr;
}

}}} // namespace boost::python::detail

#endif // UNWRAP_WRAPPER_DWA2004723_HPP
