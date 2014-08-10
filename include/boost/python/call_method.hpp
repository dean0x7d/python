// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef CALL_METHOD_DWA2002411_HPP
# define CALL_METHOD_DWA2002411_HPP

# include <boost/python/call.hpp>

namespace boost { namespace python {
    
template <typename R, typename... Args>
typename detail::returnable<R>::type 
    call_method(PyObject* self, char const* name, Args const&... args)
{
    constexpr char format[] = {'(', detail::O<Args>()..., ')', '\0'};
    
    PyObject* const result = PyEval_CallMethod(
        self,
        const_cast<char*>(name),
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
    
#endif // CALL_METHOD_DWA2002411_HPP
