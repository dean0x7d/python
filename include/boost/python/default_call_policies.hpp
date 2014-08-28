// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef DEFAULT_CALL_POLICIES_DWA2002131_HPP
# define DEFAULT_CALL_POLICIES_DWA2002131_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/python/to_python_value.hpp>
# include <boost/python/detail/value_arg.hpp>
# include <boost/python/detail/type_list_utils.hpp>

namespace boost { namespace python { 

template <class T> struct to_python_value;

struct default_result_converter;

struct default_call_policies
{
    // Ownership of this argument tuple will ultimately be adopted by
    // the caller.
    template <class ArgumentPackage>
    static bool precall(ArgumentPackage const&)
    {
        return true;
    }

    // Pass the result through
    template <class ArgumentPackage>
    static PyObject* postcall(ArgumentPackage const&, PyObject* result)
    {
        return result;
    }

    typedef default_result_converter result_converter;
    typedef PyObject* argument_package;

    template <class Sig>
    struct extract_return_type {
        using type = detail::front_t<Sig>;
    };
};

struct default_result_converter
{
    template <class R>
    struct apply
    {
        static_assert(!std::is_pointer<R>::value && !std::is_reference<R>::value,
                      "Specify a return value policy to wrap functions");

        using type = boost::python::to_python_value<
            typename detail::value_arg<R>::type
        >;
    };
};

// Exceptions for c strings an PyObject*s
template <>
struct default_result_converter::apply<char const*>
{
    typedef boost::python::to_python_value<char const*const&> type;
};

template <>
struct default_result_converter::apply<PyObject*>
{
    typedef boost::python::to_python_value<PyObject*const&> type;
};

}} // namespace boost::python

#endif // DEFAULT_CALL_POLICIES_DWA2002131_HPP
