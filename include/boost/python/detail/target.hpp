// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef TARGET_DWA2002521_HPP
# define TARGET_DWA2002521_HPP

namespace boost { namespace python { namespace detail {

template <class R>
void(* target(R (*)()) )() { return nullptr; }
template <class R, class A0, class... As>
A0(* target(R (*)(A0, As...)) )() { return nullptr; }

template <class R, class T, class... As>
T& (* target(R (T::*)(As...)) )() { return nullptr; }
template <class R, class T, class... As>
T& (* target(R (T::*)(As...) const) )() { return nullptr; }
template <class R, class T, class... As>
T& (* target(R (T::*)(As...) volatile) )() { return nullptr; }
template <class R, class T, class... As>
T& (* target(R (T::*)(As...) const volatile) )() { return nullptr; }

template <class R, class T>
T& (* target(R (T::*)) )() { return 0; }

}}} // namespace boost::python::detail

#endif // TARGET_DWA2002521_HPP
