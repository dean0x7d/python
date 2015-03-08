// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef FORWARD_DWA20011215_HPP
# define FORWARD_DWA20011215_HPP

# include <functional>
# include <boost/python/detail/value_arg.hpp>
# include <boost/python/detail/copy_ctor_mutates_rhs.hpp>

namespace boost { namespace python { namespace objects { 

template<typename T> 
struct is_reference_wrapper {
    static constexpr bool value = false;
};

template<typename T> struct is_reference_wrapper< std::reference_wrapper<T> > {
    static constexpr bool value = true;
};

template<typename T> 
struct unwrap_reference {
    typedef T type;
};

template<typename T> 
struct unwrap_reference< std::reference_wrapper<T> > {
    typedef T type;
};

}}} // namespace boost::python::objects

#endif // FORWARD_DWA20011215_HPP
