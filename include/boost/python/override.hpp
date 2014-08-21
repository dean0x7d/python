// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef OVERRIDE_DWA2004721_HPP
# define OVERRIDE_DWA2004721_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/converter/return_from_python.hpp>

# include <boost/python/extract.hpp>
# include <boost/python/handle.hpp>

#  include <boost/type.hpp>

namespace boost { namespace python {

class override;

namespace detail
{
  class wrapper_base;
  
  // The result of calling a method.
  class method_result
  {
   private:
      friend class boost::python::override;
      explicit method_result(PyObject* x)
        : m_obj(x)
      {}

   public:
      template <class T>
      operator T()
      {
          converter::return_from_python<T> converter;
          return converter(m_obj.release());
      }

#  if BOOST_WORKAROUND(_MSC_FULL_VER, BOOST_TESTED_AT(140050215))
      template <class T>
      operator T*()
      {
          converter::return_from_python<T*> converter;
          return converter(m_obj.release());
      }
#  endif 
      
#  if BOOST_WORKAROUND(BOOST_MSVC, BOOST_TESTED_AT(1400)) || BOOST_WORKAROUND(BOOST_INTEL_WIN, >= 900)
      // No operator T&
#  else
      
      template <class T>
      operator T&() const
      {
          converter::return_from_python<T&> converter;
          return converter(const_cast<handle<>&>(m_obj).release());
      }
#  endif 

      template <class T>
      T as(type<T>* = 0)
      {
          converter::return_from_python<T> converter;
          return converter(m_obj.release());
      }

      template <class T>
      T unchecked(type<T>* = 0)
      {
          return extract<T>(m_obj.get())();
      }
   private:
      mutable handle<> m_obj;
  };
}

class override : public object
{
 private:
    friend class detail::wrapper_base;
    override(handle<> x)
      : object(x)
    {}
    
 public:
    template<class... Args>
    detail::method_result operator()(Args const&... args) const
    {
        detail::method_result x(
            PyObject_CallFunctionObjArgs(
                this->ptr(),
                converter::arg_to_python<Args>(args).get()...,
                nullptr
            )
        );
        return x;
    }

};

}} // namespace boost::python

#endif // OVERRIDE_DWA2004721_HPP
