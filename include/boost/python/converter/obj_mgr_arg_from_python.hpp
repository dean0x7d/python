// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef OBJ_MGR_ARG_FROM_PYTHON_DWA2002628_HPP
# define OBJ_MGR_ARG_FROM_PYTHON_DWA2002628_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/python/detail/referent_storage.hpp>
# include <boost/python/converter/object_manager.hpp>
# include <boost/python/detail/raw_pyobject.hpp>

//
// arg_from_python converters for Python type wrappers, to be used as
// base classes for specializations.
//
namespace boost { namespace python { namespace converter { 

template <class T>
struct object_manager_value_arg_from_python {
    using result_type = T;
    
    object_manager_value_arg_from_python(PyObject* p) : m_source{p} {}
    bool convertible() const { return object_manager_traits<T>::check(m_source); }
    T operator()() const { return T(python::detail::borrowed_reference(m_source)); }

private:
    PyObject* m_source;
};

// Used for converting reference-to-object-manager arguments from
// python. The process used here is a little bit odd. Upon
// construction, we build the object manager object in the m_result
// object, *forcing* it to accept the source Python object by casting
// its pointer to detail::borrowed_reference. This is supposed to
// bypass any type checking of the source object. The convertible
// check then extracts the owned object and checks it. If the check
// fails, nothing else in the program ever gets to touch this strange
// "forced" object.
template <class Ref>
struct object_manager_ref_arg_from_python {
    using result_type = Ref;
    
    object_manager_ref_arg_from_python(PyObject* p) {
        using type = cpp14::remove_cv_t<cpp14::remove_reference_t<Ref>>;
        new (m_result.bytes) type(python::detail::borrowed_reference(p));
    }

    ~object_manager_ref_arg_from_python() {
        python::detail::destroy_stored<Ref>(m_result.bytes);
    }

    bool convertible() const {
        using type = cpp14::remove_cv_t<cpp14::remove_reference_t<Ref>>;
        return object_manager_traits<type>::check(
            get_managed_object(python::detail::void_ptr_to_reference<Ref>(m_result.bytes))
        );
    }

    Ref operator()() const {
        return python::detail::void_ptr_to_reference<Ref>(m_result.bytes);
    }

private:
    python::detail::aligned_storage<Ref> m_result;
};

}}} // namespace boost::python::converter

#endif // OBJ_MGR_ARG_FROM_PYTHON_DWA2002628_HPP
