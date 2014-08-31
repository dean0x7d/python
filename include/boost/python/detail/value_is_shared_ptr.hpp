// Copyright David Abrahams 2003.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef VALUE_IS_SHARED_PTR_DWA2003224_HPP
# define VALUE_IS_SHARED_PTR_DWA2003224_HPP

# include <boost/python/detail/is_xxx.hpp>
# include <boost/shared_ptr.hpp>

namespace boost { namespace python { namespace detail { 

template<class T>
using value_is_shared_ptr = is_<shared_ptr, 
    cpp14::remove_cv_t<
        cpp14::remove_reference_t<T>
    >
>;
    
}}} // namespace boost::python::detail

#endif // VALUE_IS_SHARED_PTR_DWA2003224_HPP
