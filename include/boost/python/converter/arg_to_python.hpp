// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef ARG_TO_PYTHON_DWA200265_HPP
# define ARG_TO_PYTHON_DWA200265_HPP

# include <boost/python/cpp14/type_traits.hpp>

# include <boost/python/ptr.hpp>
# include <boost/python/to_python_indirect.hpp>

# include <boost/python/converter/registered.hpp>
# include <boost/python/converter/arg_to_python_base.hpp>
# include <boost/python/converter/shared_ptr_to_python.hpp>
// Bring in specializations
# include <boost/python/converter/builtin_converters.hpp>

# include <boost/python/object/function_handle.hpp>

# include <boost/python/base_type_traits.hpp>

# include <boost/python/detail/string_literal.hpp>
# include <boost/python/detail/value_is_shared_ptr.hpp>
# include <boost/python/detail/unwrap.hpp>

namespace boost { namespace python { namespace converter { 

namespace detail
{
  template <class T>
  struct function_arg_to_python : handle<>
  {
      function_arg_to_python(T const& x);
  };

  template <class T>
  struct reference_arg_to_python : handle<>
  {
      reference_arg_to_python(T& x);
   private:
      static PyObject* get_object(T& x);
  };

  template <class T>
  struct shared_ptr_arg_to_python : handle<>
  {
      shared_ptr_arg_to_python(T const& x);
   private:
      static PyObject* get_object(T& x);
  };

  template <class T>
  struct value_arg_to_python : arg_to_python_base
  {
      // Throw an exception if the conversion can't succeed
      value_arg_to_python(T const&);
  };

  template <class Ptr>
  struct pointer_deep_arg_to_python : arg_to_python_base
  {
      // Throw an exception if the conversion can't succeed
      pointer_deep_arg_to_python(Ptr);
  };

  template <class Ptr>
  struct pointer_shallow_arg_to_python : handle<>
  {
      // Throw an exception if the conversion can't succeed
      pointer_shallow_arg_to_python(Ptr);
   private:
      static PyObject* get_object(Ptr p);
  };

  // Convert types that manage a Python object to_python
  template <class T>
  struct object_manager_arg_to_python
  {
      object_manager_arg_to_python(T const& x) : m_src(x) {}
      
      PyObject* get() const
      {
          return python::upcast<PyObject>(get_managed_object(m_src));
      }
      
   private:
      T const& m_src;
  };

  template <class T>
  struct select_arg_to_python
  {
      using unwrapped_type = python::detail::unwrap_t<T>;
      typedef typename unwrap_pointer<T>::type unwrapped_ptr;

      using type = cpp14::conditional_t<
          // Special handling for char const[N]; interpret them as char
          // const* for the sake of conversion
          python::detail::is_string_literal<T const>::value,
          arg_to_python<char const*>,

          cpp14::conditional_t<
              python::detail::value_is_shared_ptr<T>::value,
              shared_ptr_arg_to_python<T>,
      
              cpp14::conditional_t<
                  (std::is_function<T>::value ||
                   std::is_function<cpp14::remove_pointer_t<T>>::value ||
                   std::is_member_function_pointer<T>::value),
                  function_arg_to_python<T>,

                  cpp14::conditional_t<
                      is_object_manager<T>::value,
                      object_manager_arg_to_python<T>,

                      cpp14::conditional_t<
                          std::is_pointer<T>::value,
                          pointer_deep_arg_to_python<T>,

                          cpp14::conditional_t<
                              is_pointer_wrapper<T>::value,
                              pointer_shallow_arg_to_python<unwrapped_ptr>,

                              cpp14::conditional_t<
                                  python::detail::is_<std::reference_wrapper, T>::value,
                                  reference_arg_to_python<unwrapped_type>,
                                  value_arg_to_python<T>
                              >
                          >
                      >
                  >
              >
          >
      >;
  };

  template <class T>
  using select_arg_to_python_t = typename select_arg_to_python<T>::type;
}

template <class T>
struct arg_to_python : detail::select_arg_to_python_t<T>
{
    using base = detail::select_arg_to_python_t<T>;
 public: // member functions
    // Throw an exception if the conversion can't succeed
    arg_to_python(T const& x);
};

//
// implementations
//
namespace detail
{
  template <class T>
  inline void reject_raw_object_ptr(T*)
  {
      using base_type_trait = base_type_traits<cpp14::remove_cv_t<T>>;
      using python::detail::unspecialized;
      
      static_assert(std::is_convertible<T*, PyObject const volatile*>::value == false
                    && std::is_convertible<base_type_trait*, unspecialized*>::value,
                    "Passing a raw Python object pointer is not allowed");
  }
      
  template <class T>
  inline function_arg_to_python<T>::function_arg_to_python(T const& x)
      : handle<>(python::objects::make_function_handle(x))
  {
  }

  template <class T>
  inline value_arg_to_python<T>::value_arg_to_python(T const& x)
      : arg_to_python_base(&x, registered<T>::converters)
  {
  }

  template <class Ptr>
  inline pointer_deep_arg_to_python<Ptr>::pointer_deep_arg_to_python(Ptr x)
      : arg_to_python_base(x, registered_pointee<Ptr>::converters)
  {
      detail::reject_raw_object_ptr((Ptr)0);
  }

  template <class T>
  inline PyObject* reference_arg_to_python<T>::get_object(T& x)
  {
      to_python_indirect<T&,python::detail::make_reference_holder> convert;
      return convert(x);
  }

  template <class T>
  inline reference_arg_to_python<T>::reference_arg_to_python(T& x)
      : handle<>(reference_arg_to_python<T>::get_object(x))
  {
  }

  template <class T>
  inline shared_ptr_arg_to_python<T>::shared_ptr_arg_to_python(T const& x)
      : handle<>(shared_ptr_to_python(x))
  {
  }

  template <class Ptr>
  inline pointer_shallow_arg_to_python<Ptr>::pointer_shallow_arg_to_python(Ptr x)
      : handle<>(pointer_shallow_arg_to_python<Ptr>::get_object(x))
  {
      detail::reject_raw_object_ptr((Ptr)0);
  }

  template <class Ptr>
  inline PyObject* pointer_shallow_arg_to_python<Ptr>::get_object(Ptr x)
  {
      to_python_indirect<Ptr,python::detail::make_reference_holder> convert;
      return convert(x);
  }
}

template <class T>
inline arg_to_python<T>::arg_to_python(T const& x)
    : base(x)
{}

}}} // namespace boost::python::converter

#endif // ARG_TO_PYTHON_DWA200265_HPP
