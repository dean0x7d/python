// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/python/converter/from_python.hpp>
#include <boost/python/object/find_instance.hpp>
#include <boost/python/handle.hpp>

#include <vector>
#include <algorithm>

namespace boost { namespace python { namespace converter { 

// rvalue_from_python_stage1 -- do the first stage of a conversion
// from a Python object to a C++ rvalue.
//
//    source     - the Python object to be converted
//    converters - the registry entry for the target type T
//
// Postcondition: where x is the result, one of:
//
//   1. x.convertible == 0, indicating failure
//
//   2. x.construct == 0, x.convertible is the address of an object of
//      type T. Indicates a successful lvalue conversion
//
//   3. where y is of type rvalue_from_python_data<T>,
//      x.construct(source, y) constructs an object of type T
//      in y.storage.bytes and then sets y.convertible == y.storage.bytes,
//      or else throws an exception and has no effect.
BOOST_PYTHON_DECL rvalue_from_python_stage1_data rvalue_from_python_stage1(
    PyObject* source, registration const& converters
) {
    rvalue_from_python_stage1_data data;

    // First check to see if it's embedded in an extension class
    // instance, as a special case.
    data.convertible = objects::find_instance_impl(
        source, converters.target_type, converters.is_shared_ptr
    );
    data.construct = nullptr;

    if (!data.convertible) {
        for (auto const& rvalue_converter : converters.rvalue_chain) {
            if (auto result = rvalue_converter.convertible(source)) {
                data.convertible = result;
                data.construct = rvalue_converter.construct;
                break;
            }
        }
    }

    return data;
}

// rvalue_result_from_python -- return the address of a C++ object which
// can be used as the result of calling a Python function.
//
//      src  - the Python object to be converted
//
//      data - a reference to the base part of a
//             rvalue_from_python_data<T> object, where T is the
//             target type of the conversion.
//
// Requires: data.convertible == &registered<T>::converters
//
BOOST_PYTHON_DECL void* rvalue_result_from_python(PyObject* src,
                                                  rvalue_from_python_stage1_data& data)
{
    // Retrieve the registration
    // Cast in two steps for less-capable compilers
    auto& converters = *static_cast<registration const*>(data.convertible);

    // Look for an eligible converter
    data = rvalue_from_python_stage1(src, converters);
    return rvalue_from_python_stage2(src, data, converters);
}

BOOST_PYTHON_DECL void* rvalue_from_python_stage2(PyObject* source,
                                                  rvalue_from_python_stage1_data& data,
                                                  registration const& converters)
{
    if (!data.convertible) {
        PyErr_Format(
            PyExc_TypeError,
            "No registered converter was able to produce a C++ rvalue of type %s"
            " from this Python object of type %s",
            converters.target_type.name(),
            source->ob_type->tp_name
        );
        throw_error_already_set();
    }

    // If a construct function was registered (i.e. we found an
    // rvalue conversion), call it now.
    if (data.construct)
        data.construct(source, &data);

    // Return the address of the resulting C++ object
    return data.convertible;
}

BOOST_PYTHON_DECL void* get_lvalue_from_python(PyObject* source, registration const& converters) {
    // Check to see if it's embedded in a class instance
    void* x = objects::find_instance_impl(source, converters.target_type);
    if (x)
        return x;

    for (auto& lvalue_converter : converters.lvalue_chain) {
        if (void* result = lvalue_converter.convert(source))
            return result;
    }
    return nullptr;
}

BOOST_PYTHON_DECL bool implicit_rvalue_convertible_from_python(PyObject* source,
                                                               registration const& converters)
{    
    if (objects::find_instance_impl(source, converters.target_type))
        return true;
    
    if (converters.is_being_visited)
        return false;
    converters.is_being_visited = true;

    auto is_convertible = std::any_of(
        converters.rvalue_chain.begin(), converters.rvalue_chain.end(),
        [source](rvalue_from_python const& rvalue_converter) {
            return rvalue_converter.convertible(source);
        }
    );

    converters.is_being_visited = false;
    return is_convertible;
}

namespace
{
  void throw_no_lvalue_from_python(PyObject* source, registration const& converters,
                                   char const* ref_type)
  {
      PyErr_Format(
          PyExc_TypeError,
          "No registered converter was able to extract a C++ %s to type %s"
          " from this Python object of type %s",
          ref_type,
          converters.target_type.name(),
          source->ob_type->tp_name
      );
      throw_error_already_set();
  }

  void* lvalue_result_from_python(PyObject* source, registration const& converters,
                                  char const* ref_type)
  {
      handle<> holder(source);
      if (source->ob_refcnt <= 1) {
          PyErr_Format(
              PyExc_ReferenceError,
              "Attempt to return dangling %s to object of type: %s",
              ref_type,
              converters.target_type.name()
          );
          throw_error_already_set();
      }
      
      void* result = get_lvalue_from_python(source, converters);
      if (!result)
          throw_no_lvalue_from_python(source, converters, ref_type);
      return result;
  }
  
}

BOOST_PYTHON_DECL void throw_no_pointer_from_python(PyObject* source,
                                                    registration const& converters)
{
    throw_no_lvalue_from_python(source, converters, "pointer");
}

BOOST_PYTHON_DECL void throw_no_reference_from_python(PyObject* source,
                                                      registration const& converters)
{
    throw_no_lvalue_from_python(source, converters, "reference");
}

BOOST_PYTHON_DECL void* reference_result_from_python(PyObject* source,
                                                     registration const& converters)
{
    return lvalue_result_from_python(source, converters, "reference");
}
  
BOOST_PYTHON_DECL void* pointer_result_from_python(PyObject* source,
                                                   registration const& converters)
{
    if (source == Py_None) {
        Py_DECREF(source);
        return nullptr;
    }
    return lvalue_result_from_python(source, converters, "pointer");
}
  
BOOST_PYTHON_DECL void void_result_from_python(PyObject* o)
{
    Py_DECREF(expect_non_null(o));
}

} // namespace boost::python::converter

BOOST_PYTHON_DECL PyObject*
pytype_check(PyTypeObject* type_, PyObject* source)
{
    if (!PyObject_IsInstance(source, python::upcast<PyObject>(type_)))
    {
        ::PyErr_Format(
            PyExc_TypeError
            , "Expecting an object of type %s; got an object of type %s instead"
            , type_->tp_name
            , source->ob_type->tp_name
            );
        throw_error_already_set();
    }
    return source;
}

}} // namespace boost::python
