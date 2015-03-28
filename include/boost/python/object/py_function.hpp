// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef PY_FUNCTION_DWA200286_HPP
# define PY_FUNCTION_DWA200286_HPP

# include <boost/python/detail/signature.hpp>
# include <memory>

namespace boost { namespace python { namespace objects {

// This type is used as a "generalized Python callback", wrapping the
// function signature:
//
//      PyObject* (PyObject* args, PyObject* keywords)

struct py_function_impl_base {
    virtual ~py_function_impl_base() = default;
    virtual PyObject* operator()(PyObject*, PyObject*) = 0;
    virtual unsigned min_arity() const = 0;
    virtual unsigned max_arity() const { return min_arity(); }
    virtual python::detail::py_func_sig_info signature() const = 0;
};

template <class Caller>
struct caller_py_function_impl : py_function_impl_base {
    caller_py_function_impl(Caller const& caller)
        : m_caller(caller)
    {}

    virtual PyObject* operator()(PyObject* args, PyObject* kw) final {
        return m_caller(args, kw);
    }
    
    virtual unsigned min_arity() const final {
        return m_caller.min_arity();
    }
    
    virtual python::detail::py_func_sig_info signature() const final {
        return m_caller.signature();
    }

private:
    Caller m_caller;
};

template <class Caller, class Sig>
struct signature_py_function_impl : py_function_impl_base {
    signature_py_function_impl(Caller const& caller)
        : m_caller(caller)
    {}

    virtual PyObject* operator()(PyObject* args, PyObject* kw) final {
        return m_caller(args, kw);
    }

    virtual unsigned min_arity() const final {
        return Sig::size - 1;
    }
    
    virtual python::detail::py_func_sig_info signature() const final {
        auto const* sig = python::detail::signature<Sig>::elements();
        return {sig, sig};
    }

private:
    Caller m_caller;
};

template <class Caller, class Sig>
struct full_py_function_impl : py_function_impl_base {
    full_py_function_impl(Caller const& caller, unsigned min_arity, unsigned max_arity)
      : m_caller(caller)
      , m_min_arity(min_arity)
      , m_max_arity(max_arity > min_arity ? max_arity : min_arity)
    {}

    virtual PyObject* operator()(PyObject* args, PyObject* kw) final {
        return m_caller(args, kw);
    }

    virtual unsigned min_arity() const final {
        return m_min_arity;
    }
    
    virtual unsigned max_arity() const final {
        return m_max_arity;
    }

    virtual python::detail::py_func_sig_info signature() const final {
        auto const* sig = python::detail::signature<Sig>::elements();
        return {sig, sig};
    }

private:
    Caller m_caller;
    unsigned m_min_arity;
    unsigned m_max_arity;
};

struct py_function {
    template <class Caller>
    py_function(Caller const& caller)
        : m_impl(new caller_py_function_impl<Caller>(caller))
    {}

    template <class Caller, class Sig>
    py_function(Caller const& caller, Sig)
      : m_impl(new signature_py_function_impl<Caller, Sig>(caller))
    {}

    template <class Caller, class Sig>
    py_function(Caller const& caller, Sig, unsigned min_arity, unsigned max_arity = 0)
      : m_impl(new full_py_function_impl<Caller, Sig>(caller, min_arity, max_arity))
    {}

    PyObject* operator()(PyObject* args, PyObject* kw) const {
        return (*m_impl)(args, kw);
    }

    unsigned min_arity() const {
        return m_impl->min_arity();
    }
    
    unsigned max_arity() const {
        return m_impl->max_arity();
    }

    python::detail::signature_element const* signature() const {
        return m_impl->signature().signature;
    }

    python::detail::signature_element const& get_return_type() const {
        return *m_impl->signature().ret;
    }
    
private:
    std::unique_ptr<py_function_impl_base> m_impl;
};

}}} // namespace boost::python::objects

#endif // PY_FUNCTION_DWA200286_HPP
