// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RESULT_DWA2002521_HPP
# define RESULT_DWA2002521_HPP

# include <boost/type.hpp>

namespace boost { namespace python { namespace detail {

// Defines a family of overloaded function which, given x, a function
// pointer, member [function] pointer, or an AdaptableFunction object,
// returns a pointer to type<R>*, where R is the result type of
// invoking the result of bind(x).
//
// In order to work around bugs in deficient compilers, if x might be
// an AdaptableFunction object, you must pass OL as a second argument
// to get this to work portably.

template <class R, class... As>
boost::type<R>* result(R (*)(As...), int = 0) { return nullptr; }

template <class R, class T, class... As>
boost::type<R>* result(R (T::*)(As...), int = 0) { return nullptr; }

template <class R, class T, class... As>
boost::type<R>* result(R (T::*)(As...) const, int = 0) { return nullptr; }
template <class R, class T, class... As>
boost::type<R>* result(R (T::*)(As...) volatile, int = 0) { return nullptr; }
template <class R, class T, class... As>
boost::type<R>* result(R (T::*)(As...) const volatile, int = 0) { return nullptr; }

template <class R, class T>
boost::type<R>* result(R (T::*), int = 0) { return nullptr; }

template <class X>
boost::type<typename X::result_type>* result(X const&, short = 0) { return nullptr; }

}}} // namespace boost::python::detail

#endif // RESULT_DWA2002521_HPP
