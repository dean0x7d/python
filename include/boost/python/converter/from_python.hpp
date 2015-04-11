// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef FIND_FROM_PYTHON_DWA2002223_HPP
# define FIND_FROM_PYTHON_DWA2002223_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/python/converter/rvalue_from_python_data.hpp>

# include <boost/python/converter/registered.hpp>
# include <boost/python/errors.hpp>

namespace boost { namespace python { namespace converter { 

BOOST_PYTHON_DECL
void* get_lvalue_from_python(PyObject* source, registration const&);

BOOST_PYTHON_DECL bool implicit_rvalue_convertible_from_python(PyObject* source,
                                                               registration const&);

BOOST_PYTHON_DECL rvalue_from_python_stage1_data rvalue_from_python_stage1(PyObject* source,
                                                                           registration const&);

BOOST_PYTHON_DECL void* rvalue_from_python_stage2(PyObject* source,
                                                  rvalue_from_python_stage1_data&,
                                                  registration const&);

BOOST_PYTHON_DECL void* rvalue_result_from_python(PyObject*,
                                                  rvalue_from_python_stage1_data&);

namespace errors {
    BOOST_PYTHON_DECL
    void throw_dangling_pointer(registration const&, char const* ref_type);

    BOOST_PYTHON_DECL
    void throw_bad_conversion(PyObject* source, registration const&, char const* ref_type);
};

// lvalue converters
//
//   These require that an lvalue of the type U is stored somewhere in
//   the Python object being converted.
//
template<class T>
struct lvalue_from_python;

template<class T>
struct lvalue_from_python<T*> {
    lvalue_from_python(PyObject* source)
        : result{(source == Py_None)
                 ? source
                 : get_lvalue_from_python(source, registered<T>::converters)}
    {}

    bool check() const { return result != nullptr; }

    T* operator()() const {
        return (result == Py_None) ? nullptr : static_cast<T*>(result);
    }

public:
    static void throw_dangling_pointer() {
        errors::throw_dangling_pointer(registered<T>::converters, "pointer");
    }

    static void throw_bad_conversion(PyObject* source) {
        errors::throw_bad_conversion(source, registered<T>::converters, "pointer");
    }

private:
    void* result;
};

template<class T>
struct lvalue_from_python<T&> {
    lvalue_from_python(PyObject* source)
        : result{get_lvalue_from_python(source, registered<T>::converters)}
    {}

    bool check() const { return result != nullptr; }

    T& operator()() const {
        return python::detail::void_ptr_to_reference<T>(result);
    }

public:
    static void throw_dangling_pointer() {
        errors::throw_dangling_pointer(registered<T>::converters, "reference");
    }

    static void throw_bad_conversion(PyObject* source) {
        errors::throw_bad_conversion(source, registered<T>::converters, "reference");
    }

private:
    void* result;
};

}}} // namespace boost::python::converter

#endif // FIND_FROM_PYTHON_DWA2002223_HPP
