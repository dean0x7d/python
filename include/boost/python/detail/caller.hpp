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
# include <boost/python/converter/arg_from_python.hpp>
# include <boost/python/converter/context_result_converter.hpp>
# include <boost/python/converter/builtin_converters.hpp>

namespace boost { namespace python { namespace detail { 

// A dummy converter is required for void.
template <class CallPolicies, class Result>
struct select_result_converter {
    using type = typename CallPolicies::result_converter::template apply<Result>::type;
};

template <class CallPolicies>
struct select_result_converter<CallPolicies, void> {
    struct type {
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        static PyTypeObject const* get_pytype() { return nullptr; }
#endif
    };
};

template <class CallPolicies, class Result>
using select_result_converter_t = typename select_result_converter<CallPolicies, Result>::type;


template<class ResultConverter>
inline ResultConverter create_result_converter_impl(PyObject* args, std::true_type) {
    return {args};
}

template<class ResultConverter>
inline ResultConverter create_result_converter_impl(PyObject* /*args*/, std::false_type) {
    return {};
}

template<class CallPolicies, class Result,
         class ResultConverter = select_result_converter_t<CallPolicies, Result>>
inline ResultConverter create_result_converter(PyObject* args) {
    using pick = std::is_base_of<converter::context_result_converter, ResultConverter>;
    return create_result_converter_impl<ResultConverter>(args, pick{});
}


// A function object type which wraps C++ objects as Python callable
// objects.
//
// Template Arguments:
//
//   Function -
//      the C++ `function object' that will be called. Might
//      actually be any data for which an appropriate invoke_tag can
//      be generated. invoke(...) takes care of the actual invocation syntax.
//
//   CallPolicies -
//      The precall, postcall, and what kind of resultconverter to
//      generate for Result
//
//   Signature -
//      The `intended signature' of the function. A type_list
//      beginning with a result type and continuing with a list of
//      argument types.
template<class Function, class CallPolicies, class Signature,
         class = cpp14::make_index_sequence<Signature::size - 1>>
struct caller;

template<class Function, class CallPolicies, class Result, class... Args, std::size_t... Is>
struct caller<Function, CallPolicies, type_list<Result, Args...>, cpp14::index_sequence<Is...>>
    : CallPolicies // inherit to take advantage of empty base class optimisation
{
    caller(Function f, CallPolicies const& cp) : CallPolicies(cp), m_function(f) {}

    PyObject* operator()(PyObject* args, PyObject* /*kwargs*/) {
        auto arg_pack = argument_package{args};
        return call_impl(arg_pack, converter::arg_from_python<Args>(arg_pack.get(Is))...);
    }
    
    static unsigned min_arity() { return sizeof...(Args); }
    
    static py_func_sig_info signature() {
        using Sig = type_list<Result, Args...>;
        signature_element const* sig = detail::signature<Sig>::elements();

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        using rtype = typename CallPolicies::template extract_return_type<Sig>::type;

        static const signature_element ret = {
            std::is_same<void, rtype>::value ? "void" : type_id<rtype>().name(),
            &select_result_converter_t<CallPolicies, Result>::get_pytype,
            std::is_reference<rtype>::value &&
                !std::is_const<cpp14::remove_reference_t<rtype>>::value
        };
        py_func_sig_info res = {sig, &ret};
#else
        py_func_sig_info res = {sig, sig};
#endif
        return res;
    }

private:
    using argument_package = typename CallPolicies::argument_package;

    template<class... Converters>
    PyObject* call_impl(argument_package arg_pack, Converters... converters) {
        // The 'true' at the end is not needed, but it keeps VS14 CTP3 happy
        bool check_converters[] = { converters.check()..., true };
        for (auto is_convertible : check_converters) {
            if (!is_convertible)
                return nullptr;
        }

        if (!CallPolicies::precall(arg_pack))
            return nullptr;

        PyObject* result = detail::invoke(
            detail::make_invoke_tag<Result, Function>{},
            create_result_converter<CallPolicies, Result>(arg_pack.base_args),
            m_function,
            std::move(converters)...
        );

        return CallPolicies::postcall(arg_pack, result);
    }

private:
    Function m_function;
};
    
}}} // namespace boost::python::detail

#endif // CALLER_DWA20021121_HPP
