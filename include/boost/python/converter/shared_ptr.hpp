// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef SHARED_PTR_FROM_PYTHON_DWA20021130_HPP
# define SHARED_PTR_FROM_PYTHON_DWA20021130_HPP

# include <boost/python/handle.hpp>

# include <boost/python/converter/shared_ptr_fwd.hpp>
# include <boost/python/converter/from_python.hpp>
# include <boost/python/converter/rvalue_from_python_data.hpp>
# include <boost/python/converter/registered.hpp>
# ifndef BOOST_PYTHON_NO_PY_SIGNATURES
#  include <boost/python/converter/pytype_function.hpp>
# endif

# include <boost/python/detail/none.hpp>

namespace boost { namespace python { namespace converter {

struct shared_ptr_deleter {
    shared_ptr_deleter(handle<> owner) : owner(owner) {}

    void operator()(void const*) {
        owner.reset();
    }

    handle<> owner;
};

template<class T>
PyObject* shared_ptr_to_python(shared_ptr<T> const& x) {
    if (!x)
        return python::detail::none();
    else if (auto* d = get_deleter<shared_ptr_deleter>(x))
        return incref(d->owner.get());
    else
        return converter::registered<shared_ptr<T>>::converters.to_python(&x);
}

template <class T>
struct shared_ptr_from_python {
    shared_ptr_from_python() {
        converter::registry::insert(
            &convertible, &construct, type_id<shared_ptr<T>>(),
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
            &converter::expected_from_python_type_direct<T>::get_pytype
#endif
        );
    }

private:
    static void* convertible(PyObject* p) {
        if (p == Py_None)
            return p;
        else
            return converter::get_lvalue_from_python(p, registered<T>::converters);
    }
    
    static void construct(PyObject* source, rvalue_from_python_stage1_data* data) {
        void* const storage = ((converter::rvalue_from_python_storage<shared_ptr<T>>*)
            data)->storage.bytes;

        if (data->convertible == Py_None) {
            new (storage) shared_ptr<T>{};
        }
        else {
            // workaround for enable_shared_from_this
            shared_ptr<void> hold_convertible_ref_count{
                nullptr,
                shared_ptr_deleter{handle<>{borrowed(source)}}
            };
            // use aliasing constructor
            new (storage) shared_ptr<T>{
                hold_convertible_ref_count,
                static_cast<T*>(data->convertible)
            };
        }
        
        data->convertible = storage;
    }
};

}}} // namespace boost::python::converter

#endif // SHARED_PTR_FROM_PYTHON_DWA20021130_HPP
