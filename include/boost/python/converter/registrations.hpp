// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef REGISTRATIONS_DWA2002223_HPP
# define REGISTRATIONS_DWA2002223_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/type_id.hpp>

# include <boost/python/converter/convertible_function.hpp>
# include <boost/python/converter/constructor_function.hpp>
# include <boost/python/converter/to_python_function_type.hpp>

# include <forward_list>

namespace boost { namespace python { namespace converter { 

struct lvalue_from_python {
    convertible_function convert;
};

struct rvalue_from_python {
    convertible_function convertible;
    constructor_function construct;
    PyTypeObject const* (*expected_pytype)();
};

struct BOOST_PYTHON_DECL registration
{
 public: // member functions
    explicit registration(type_info target, bool is_shared_ptr = false)
        : target_type(target), is_shared_ptr(is_shared_ptr) {}

    // Convert the appropriately-typed data to Python
    PyObject* to_python(void const volatile*) const;

    // Return the class object, or raise an appropriate Python
    // exception if no class has been registered.
    PyTypeObject* get_class_object() const;

    // Return common denominator of the python class objects, 
    // convertable to target. Inspects the m_class_object and the value_chains.
    PyTypeObject const* expected_from_python_type() const;
    PyTypeObject const* to_python_target_type() const;

 public: // data members. So sue me.
    const python::type_info target_type;

    // The chain of eligible from_python converters when an lvalue is required
    std::forward_list<lvalue_from_python> lvalue_chain;

    // The chain of eligible from_python converters when an rvalue is acceptable
    std::forward_list<rvalue_from_python> rvalue_chain;

    // The class object associated with this type
    PyTypeObject* m_class_object = nullptr;

    // The unique to_python converter for the associated C++ type.
    to_python_function_t m_to_python = nullptr;
    PyTypeObject const* (*m_to_python_target_type)() = nullptr;

    // True iff this type is a shared_ptr.  Needed for special rvalue
    // from_python handling.
    const bool is_shared_ptr;
};

inline bool operator<(registration const& lhs, registration const& rhs)
{
    return lhs.target_type < rhs.target_type;
}

}}} // namespace boost::python::converter

#endif // REGISTRATIONS_DWA2002223_HPP
