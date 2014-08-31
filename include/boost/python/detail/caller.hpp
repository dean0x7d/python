// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CALLER_DWA20021121_HPP
# define CALLER_DWA20021121_HPP

# include <boost/python/cpp14/utility.hpp>
# include <boost/python/cpp14/type_traits.hpp>

# include <boost/python/detail/invoke.hpp>
# include <boost/python/detail/signature.hpp>

# include <boost/python/type_id.hpp>
# include <boost/python/arg_from_python.hpp>
# include <boost/python/converter/context_result_converter.hpp>
# include <boost/python/converter/builtin_converters.hpp>

namespace boost { namespace python { namespace detail { 

template <int N>
inline PyObject* get(PyObject* const& args) {
    return PyTuple_GET_ITEM(args, N);
}

inline Py_ssize_t arity(PyObject* const& args) {
    return PyTuple_GET_SIZE(args);
}

// This "result converter" is really just used as
// a dispatch tag to invoke(...), selecting the appropriate
// implementation
typedef int void_result_to_python;

// Given a model of CallPolicies and a C++ result type, this
// metafunction selects the appropriate converter to use for
// converting the result to python.
template <class Policies, class Result, bool is_void = true>
struct select_result_converter {
    using type = void_result_to_python;
};

template <class Policies, class Result>
struct select_result_converter<Policies, Result, false> {
    using type = typename Policies::result_converter::template apply<Result>::type;
};

template <class Policies, class Result>
using select_result_converter_t = typename select_result_converter<
    Policies, Result, std::is_same<Result, void>::value
>::type;


template <class ResultConverter, class ArgPackage>
inline ResultConverter create_result_converter(ArgPackage const& args, std::true_type) {
    return {args};
}

template <class ResultConverter, class ArgPackage>
inline ResultConverter create_result_converter(ArgPackage const& args, std::false_type) {
    return {};
}

template <class ResultConverter, class ArgPackage>
inline ResultConverter create_result_converter(ArgPackage const& args) {
    using pick = std::is_convertible<ResultConverter*, converter::context_result_converter*>;
    return create_result_converter<ResultConverter>(args, pick());
}

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
template <class ResultConverter>
struct converter_target_type 
{
    static PyTypeObject const *get_pytype()
    {
        return create_result_converter<ResultConverter>((PyObject*)nullptr).get_pytype();
    }
};

template < >
struct converter_target_type <void_result_to_python >
{
    static PyTypeObject const *get_pytype()
    {
        return nullptr;
    }
};
#endif


// A function object type which wraps C++ objects as Python callable
// objects.
//
// Template Arguments:
//
//   F -
//      the C++ `function object' that will be called. Might
//      actually be any data for which an appropriate invoke_tag() can
//      be generated. invoke(...) takes care of the actual invocation syntax.
//
//   CallPolicies -
//      The precall, postcall, and what kind of resultconverter to
//      generate for Result
//
//   Sig -
//      The `intended signature' of the function. A type_list
//      beginning with a result type and continuing with a list of
//      argument types.
template <class F, class CallPolicies, class Sig> 
struct caller;

template <class F, class CallPolicies, class Result, class... Args>
struct caller<F, CallPolicies, type_list<Result, Args...>>
{
    caller(F f) : m_function(f) {}
    
    using argument_package = typename CallPolicies::argument_package;

    template <std::size_t... Is>
    bool check_converters(argument_package pack, cpp14::index_sequence<Is...>)
    {
        // The 'true' at the end is not needed, but it keeps VS14 CTP3 happy
        bool chk[] = { arg_from_python<Args>(get<Is>(pack)).convertible()..., true };
        for (auto is_convertible : chk) {
            if (!is_convertible)
                return false;
        }
        return true;
    }
    
    template <std::size_t... Is>
    PyObject* call_impl(argument_package inner_args, PyObject* args, cpp14::index_sequence<Is...>)
    {
        using result_converter = select_result_converter_t<CallPolicies, Result>;
        
        return detail::invoke(
            detail::invoke_tag<Result, F>(),
            create_result_converter<result_converter>(args),
            m_function,
            arg_from_python<Args>(get<Is>(inner_args))...
        );
    }
    
    PyObject* operator()(PyObject* args, PyObject*) // eliminate
                                                     // this
                                                     // trailing
                                                     // keyword dict
    {
        using argument_package = typename CallPolicies::argument_package;
        argument_package inner_args(args);
        
        if (!check_converters(inner_args, cpp14::index_sequence_for<Args...>()))
            return nullptr;
        
        // all converters have been checked. Now we can do the
        // precall part of the policy
        if (!CallPolicies::precall(inner_args))
            return nullptr;
        
        // TODO: Currently, the converters are created twice (check_converters, call_impl).
        //       This can be improved by making a tuple<converters...> and passing it to both.
        PyObject* result = call_impl(inner_args, args, cpp14::index_sequence_for<Args...>());
        
        return CallPolicies::postcall(inner_args, result);
    }
    
    static unsigned min_arity() { return sizeof...(Args); }
    
    static py_func_sig_info signature()
    {
        using Sig = type_list<Result, Args...>;
        
        const signature_element * sig = detail::signature<Sig>::elements();
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        
        typedef typename CallPolicies::template extract_return_type<Sig>::type rtype;
        using result_converter = select_result_converter_t<CallPolicies, rtype>;
        
        static const signature_element ret = {
            std::is_same<void, rtype>::value ? "void" : type_id<rtype>().name(),
            &detail::converter_target_type<result_converter>::get_pytype,
            std::is_reference<rtype>::value && 
            !std::is_const<cpp14::remove_reference_t<rtype>>::value 
        };
        py_func_sig_info res = {sig, &ret };
#else
        py_func_sig_info res = {sig, sig };
#endif
        
        return res;
    }

private:
    F m_function;
};
    
}}} // namespace boost::python::detail

#endif // CALLER_DWA20021121_HPP
