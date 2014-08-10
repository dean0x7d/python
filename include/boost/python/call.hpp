// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CALL_DWA2002411_HPP
# define CALL_DWA2002411_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/converter/arg_to_python.hpp>
# include <boost/python/converter/return_from_python.hpp>

namespace boost { namespace python {

namespace detail {
    template<typename T>
    constexpr char O() { return 'O'; }
}

template<typename R, typename... Args>
typename detail::returnable<R>::type 
    call(PyObject* callable, Args const&... args)
{
    // Format should be "(OOOO...)" with as many O's as there are arguments
    constexpr char format[] = {'(', detail::O<Args>()..., ')', '\0'};
    
    PyObject* const result = PyEval_CallFunction(
        callable, 
        const_cast<char*>(format),
        converter::arg_to_python<Args>(args).get()...
    );
    
    // This conversion *must not* be done in the same expression as
    // the call, because, in the special case where the result is a
    // reference a Python object which was created by converting a C++
    // argument for passing to PyEval_CallFunction, its reference
    // count will be 2 until the end of the full expression containing
    // the conversion, and that interferes with dangling
    // pointer/reference detection.
    converter::return_from_python<R> converter;
    return converter(result);
}

}} // namespace boost::python

#endif // CALL_DWA2002411_HPP
