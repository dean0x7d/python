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

namespace errors {
    BOOST_PYTHON_DECL
    void throw_dangling_pointer(registration const&, char const* ref_type);

    BOOST_PYTHON_DECL
    void throw_bad_lvalue_conversion(PyObject* source, registration const&, char const* ref_type);

    BOOST_PYTHON_DECL
    void throw_bad_rvalue_conversion(PyObject* source, registration const&);
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
        errors::throw_bad_lvalue_conversion(source, registered<T>::converters, "pointer");
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
        errors::throw_bad_lvalue_conversion(source, registered<T>::converters, "reference");
    }

private:
    void* result;
};

// rvalue converters
//
//   These require only that an object of type T can be created from
//   the given Python object, but not that the T object exist
//   somewhere in storage.
//
template<class T>
struct rvalue_from_python {
    rvalue_from_python(PyObject* source)
        : source{source},
          data{rvalue_from_python_stage1(source, registered<T>::converters)}
    {}

    bool check() const { return data.stage1.convertible != nullptr; }

    T& operator()() {
        // Only do the stage2 conversion once
        if (data.stage1.convertible != data.storage.bytes && data.stage1.construct)
            data.stage1.construct(source, &data.stage1);

        return python::detail::void_ptr_to_reference<T>(data.stage1.convertible);
    }

public:
    static void throw_bad_conversion(PyObject* source) {
        errors::throw_bad_rvalue_conversion(source, registered<T>::converters);
    }

protected:
    PyObject* source;
    rvalue_from_python_data<T> data;
};

}}} // namespace boost::python::converter

#endif // FIND_FROM_PYTHON_DWA2002223_HPP
