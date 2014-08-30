// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef MAKE_PTR_INSTANCE_DWA200296_HPP
# define MAKE_PTR_INSTANCE_DWA200296_HPP

# include <boost/python/object/make_instance.hpp>
# include <boost/python/converter/registry.hpp>
# include <boost/get_pointer.hpp>
# include <typeinfo>

namespace boost { namespace python { namespace objects { 

template <class T, class Holder>
struct make_ptr_instance
    : make_instance_impl<T, Holder, make_ptr_instance<T,Holder> >
{
    template <class Arg>
    static inline Holder* construct(void* storage, PyObject*, Arg& x)
    {
        return new (storage) Holder(x);
    }
    
    template <class Ptr>
    static inline PyTypeObject* get_class_object(Ptr const& x)
    {
        return get_class_object_impl(get_pointer(x));
    }
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    static inline PyTypeObject const* get_pytype()
    {
        return converter::registered<T>::converters.get_class_object();
    }
#endif
 private:
    template <class U>
    static inline PyTypeObject* get_class_object_impl(U const volatile* p)
    {
        if (p == nullptr)
            return nullptr; // means "return None".

        PyTypeObject* derived = get_derived_class_object(std::is_polymorphic<U>(), p);
        
        if (derived)
            return derived;
        return converter::registered<T>::converters.get_class_object();
    }
    
    template <class U>
    static inline PyTypeObject* get_derived_class_object(std::true_type, U const volatile* x)
    {
        converter::registration const* r = converter::registry::query(
            type_info(typeid(*get_pointer(x)))
        );
        return r ? r->m_class_object : nullptr;
    }
    
    template <class U>
    static inline PyTypeObject* get_derived_class_object(std::false_type, U*)
    {
        return nullptr;
    }
};
  

}}} // namespace boost::python::object

#endif // MAKE_PTR_INSTANCE_DWA200296_HPP
