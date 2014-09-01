// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TO_PYTHON_VALUE_DWA200221_HPP
# define TO_PYTHON_VALUE_DWA200221_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/refcount.hpp>
# include <boost/python/tag.hpp>
# include <boost/python/handle.hpp>

# include <boost/python/converter/registry.hpp>
# include <boost/python/converter/registered.hpp>
# include <boost/python/converter/builtin_converters.hpp>
# include <boost/python/converter/object_manager.hpp>
# include <boost/python/converter/shared_ptr_to_python.hpp>

# include <boost/python/detail/value_is_shared_ptr.hpp>
# include <boost/python/detail/value_arg.hpp>

namespace boost { namespace python { 

namespace detail
{
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES

template <bool is_const_ref>
struct object_manager_get_pytype
{
   template <class U>
   static PyTypeObject const* get( U& (*)() =0)
   {
      return converter::object_manager_traits<U>::get_pytype();
   }
};

template <>
struct object_manager_get_pytype<true>
{
   template <class U>
   static PyTypeObject const* get( U const& (*)() =0)
   {
      return converter::object_manager_traits<U>::get_pytype();
   }
};

#endif

  template <class T>
  struct object_manager_to_python_value
  {
      using argument_type = value_arg_t<T>;
    
      PyObject* operator()(argument_type) const;
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      PyTypeObject const* get_pytype() const {
          return get_pytype_aux(is_handle<T>());
      }

      inline static PyTypeObject const* get_pytype_aux(std::true_type) {
          return converter::object_manager_traits<T>::get_pytype();
      }
      
      inline static PyTypeObject const* get_pytype_aux(std::false_type) {
          constexpr bool is_t_const = std::is_const<cpp14::remove_reference_t<T>>::value;
          return object_manager_get_pytype<is_t_const>::get((T(*)())0);
      }
      
#endif 

      // This information helps make_getter() decide whether to try to
      // return an internal reference or not. I don't like it much,
      // but it will have to serve for now.
      static constexpr bool uses_registry = false;
  };

  
  template <class T>
  struct registry_to_python_value
  {
      using argument_type = value_arg_t<T>;
    
      PyObject* operator()(argument_type) const;
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      PyTypeObject const* get_pytype() const {return converter::registered<T>::converters.to_python_target_type();}
#endif

      // This information helps make_getter() decide whether to try to
      // return an internal reference or not. I don't like it much,
      // but it will have to serve for now.
      static constexpr bool uses_registry = true;
  };

  template <class T>
  struct shared_ptr_to_python_value
  {
      using argument_type = value_arg_t<T>;
    
      PyObject* operator()(argument_type) const;
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      PyTypeObject const* get_pytype() const {
          return converter::registered<
              typename cpp14::remove_reference_t<argument_type>::element_type
          >::converters.to_python_target_type();
      }
#endif 
      // This information helps make_getter() decide whether to try to
      // return an internal reference or not. I don't like it much,
      // but it will have to serve for now.
      static constexpr bool uses_registry = false;
  };
}

template <class T>
struct to_python_value : cpp14::conditional_t<
    detail::value_is_shared_ptr<T>::value,
    detail::shared_ptr_to_python_value<T>,
    cpp14::conditional_t<
        (converter::is_object_manager<T>::value ||
         converter::is_reference_to_object_manager<T>::value),
        detail::object_manager_to_python_value<T>,
        detail::registry_to_python_value<T>
    >
> {};
    

//
// implementation 
//
namespace detail
{
  template <class T>
  inline PyObject* registry_to_python_value<T>::operator()(argument_type x) const
  {
      return converter::registered<argument_type>::converters.to_python(&x);
  }

  template <class T>
  inline PyObject* object_manager_to_python_value<T>::operator()(argument_type x) const
  {
      return python::upcast<PyObject>(
          python::xincref(
              get_managed_object(x, tag))
          );
  }

  template <class T>
  inline PyObject* shared_ptr_to_python_value<T>::operator()(argument_type x) const
  {
      return converter::shared_ptr_to_python(x);
  }
}

}} // namespace boost::python

#endif // TO_PYTHON_VALUE_DWA200221_HPP
