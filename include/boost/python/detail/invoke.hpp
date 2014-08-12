// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef INVOKE_DWA20021122_HPP
# define INVOKE_DWA20021122_HPP

# include <boost/python/detail/none.hpp>

// This file declares a series of overloaded invoke(...)  functions,
// used to invoke wrapped C++ function (object)s from Python. Each one
// accepts:
//
//   - a tag which identifies the invocation syntax (e.g. member
//   functions must be invoked with a different syntax from regular
//   functions)
//
//   - a pointer to a result converter type, used solely as a way of
//   transmitting the type of the result converter to the function (or
//   an int, if the return type is void).
//
//   - the "function", which may be a function object, a function or
//   member function pointer, or a defaulted_virtual_fn.
//
//   - The arg_from_python converters for each of the arguments to be
//   passed to the function being invoked.

namespace boost { namespace python { namespace detail { 

template <bool void_return, bool member>
struct invoke_tag_ {};

// A metafunction returning the appropriate tag type for invoking an
// object of type F with return type R.
template <class R, class F>
struct invoke_tag
  : invoke_tag_<
    std::is_same<R,void>::value,
    std::is_member_function_pointer<F>::value
    >
{
};

template <class RC, class F, class... ACs>
inline PyObject* invoke(invoke_tag_<false,false>, RC const& rc, F& f, ACs&&... acs)
{
    return rc(f( std::forward<ACs>(acs)()... ));
}

template <class RC, class F, class... ACs>
inline PyObject* invoke(invoke_tag_<true,false>, RC const&, F& f, ACs&&... acs)
{
    f( std::forward<ACs>(acs)()... );
    return none();
}

template <class RC, class F, class TC, class... ACs>
inline PyObject* invoke(invoke_tag_<false,true>, RC const& rc, F& f, TC& tc, ACs&&... acs)
{
    return rc( (tc().*f)(std::forward<ACs>(acs)()...) );
}

template <class RC, class F, class TC, class... ACs>
inline PyObject* invoke(invoke_tag_<true,true>, RC const&, F& f, TC& tc, ACs&&... acs)
{
    (tc().*f)(std::forward<ACs>(acs)()...);
    return none();
}
    
}}} // namespace boost::python::detail

#endif // INVOKE_DWA20021122_HPP
