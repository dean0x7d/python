// Copyright David Abrahams 2005.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef IS_XXX_DWA2003224_HPP
# define IS_XXX_DWA2003224_HPP

namespace boost { namespace python { namespace detail { 

//
//  Test for any kind of template type. 
//
//  E.g.: "is_<shared_ptr, T>::value" will return true
//        if "T == shared_ptr<U>" where U is anything.
//

template<template<class...> class Class, class T>
struct is_ : std::false_type {};

template<template<class...> class Class, class... Args>
struct is_<Class, Class<Args...>> : std::true_type {};

}}} // namespace boost::python::detail

#endif // IS_XXX_DWA2003224_HPP
