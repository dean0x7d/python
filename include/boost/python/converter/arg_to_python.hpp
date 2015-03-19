// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef ARG_TO_PYTHON_DWA200265_HPP
# define ARG_TO_PYTHON_DWA200265_HPP

# include <boost/ref.hpp>
# include <boost/python/ptr.hpp>
# include <boost/python/to_python_indirect.hpp>
# include <boost/python/object/function_handle.hpp>
# include <boost/python/base_type_traits.hpp>

# include <boost/python/converter/registered.hpp>
# include <boost/python/converter/shared_ptr_to_python.hpp>
// Bring in specializations
# include <boost/python/converter/builtin_converters.hpp>

# include <boost/python/detail/string_literal.hpp>
# include <boost/python/detail/value_is_shared_ptr.hpp>
# include <boost/python/detail/unwrap.hpp>

namespace boost { namespace python { namespace converter { 

namespace detail
{
  template <class T>
  struct function_arg_to_python : handle<> {
      function_arg_to_python(T const& x)
          : handle<>{python::objects::make_function_handle(x)}
      {}
  };

  template <class T>
  struct reference_arg_to_python : handle<> {
      reference_arg_to_python(T& x)
          : handle<>{to_python_indirect<T&, python::detail::make_reference_holder>{}(x)}
      {}
  };

  template <class T>
  struct shared_ptr_arg_to_python : handle<> {
      shared_ptr_arg_to_python(T const& x)
          : handle<>(shared_ptr_to_python(x))
      {}
  };

  template <class T>
  struct value_arg_to_python : handle<>  {
      value_arg_to_python(T const& x)
          : handle<>(registered<T>::converters.to_python(&x))
      {}
  };

  template <class Ptr>
  struct pointer_deep_arg_to_python : handle<> {
      static_assert(!is_pyobject<cpp14::remove_pointer_t<Ptr>>::value,
                    "Passing a raw Python object pointer is not allowed");

      pointer_deep_arg_to_python(Ptr x)
          : handle<>(registered_pointee<Ptr>::converters.to_python(x))
      {}
  };

  template <class Ptr>
  struct pointer_shallow_arg_to_python : handle<> {
      static_assert(!is_pyobject<cpp14::remove_pointer_t<Ptr>>::value,
                    "Passing a raw Python object pointer is not allowed");

      pointer_shallow_arg_to_python(Ptr x)
          : handle<>(to_python_indirect<Ptr, python::detail::make_reference_holder>{}(x))
      {}
  };

  template <class T>
  struct object_manager_arg_to_python {
      object_manager_arg_to_python(T const& x) : m_src(x) {}
      
      PyObject* get() const {
          return python::upcast<PyObject>(get_managed_object(m_src));
      }
      
  private:
      T const& m_src;
  };

  template <class T, class unwrapped_type = python::detail::unwrap_t<T>>
  using select_arg_to_python_t = cpp14::conditional_t<
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
                          python::detail::is_<pointer_wrapper, T>::value,
                          pointer_shallow_arg_to_python<unwrapped_type>,

                          cpp14::conditional_t<
                              python::detail::is_<boost::reference_wrapper, T>::value,
                              reference_arg_to_python<unwrapped_type>,
                              value_arg_to_python<T>
                          >
                      >
                  >
              >
          >
      >
  >;
}

// Throw an exception if the conversion can't succeed
template <class T>
struct arg_to_python : detail::select_arg_to_python_t<T> {
    using detail::select_arg_to_python_t<T>::select_arg_to_python_t;
};

}}} // namespace boost::python::converter

#endif // ARG_TO_PYTHON_DWA200265_HPP
