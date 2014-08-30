// Copyright David Abrahams and Nikolay Mladenov 2003.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef RETURN_ARG_DWA2003719_HPP
# define RETURN_ARG_DWA2003719_HPP
# include <boost/python/default_call_policies.hpp>
# include <boost/python/detail/none.hpp>
# include <boost/python/detail/value_arg.hpp>

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
# include <boost/python/converter/pytype_function.hpp>
#endif

# include <boost/python/refcount.hpp>

namespace boost { namespace python { 

namespace detail
{
  struct return_none
  {
      template <class T> struct apply
      {
          struct type
          {
              static bool convertible()
              {
                  return true;
              }
              
              PyObject *operator()(value_arg_t<T>) const
              {
                  return none();
              }
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
              PyTypeObject const *get_pytype() const {
                  return converter::expected_pytype_for_arg<T>::get_pytype();
              }
#endif
          };
      };
  };
}
    
template <
    std::size_t arg_pos=1
  , class Base = default_call_policies
> 
struct return_arg : Base
{
    using result_converter = detail::return_none;
    static_assert(arg_pos > 0, "return_arg pos must be positive");
        // we could default to the base result_converter in case or
        // arg_pos==0 since return arg 0 means return result, but I
        // think it is better to issue an error instead, cause it can
        // lead to confusions

    template <class ArgumentPackage>
    static PyObject* postcall(ArgumentPackage const& args, PyObject* result)
    {
        result = Base::postcall(args,result);
        if (!result)
            return 0;
        Py_DECREF(result);
        return incref( detail::get<arg_pos-1>(args) );
    }

    template <class Sig>
    struct extract_return_type {
        using type = detail::get_t<Sig, arg_pos>;
    };
};

template <class Base = default_call_policies>
struct return_self : return_arg<1,Base>
{};

}} // namespace boost::python

#endif // RETURN_ARG_DWA2003719_HPP
