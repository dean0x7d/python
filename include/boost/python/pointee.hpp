// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef POINTEE_DWA2002323_HPP
# define POINTEE_DWA2002323_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/python/cpp14/type_traits.hpp>

namespace boost { namespace python {

namespace detail
{
  // T is a raw pointer
  template <class T, bool is_raw_pointer = true>
  struct pointee_impl {
      using type = cpp14::remove_pointer_t<T>;
  };

  // T is a smart pointer
  template <class T>
  struct pointee_impl<T, false> {
      using type = typename T::element_type;
  };
}

// T is a pointer type
template <class T, bool is_pointer = true>
struct pointee
    : detail::pointee_impl<T, std::is_pointer<T>::value>
{
};

// T is a value type
template <class T>
struct pointee<T, false> {
    using type = T;
};
    
template <class T, bool is_pointer = true>
using pointee_t = typename pointee<T, is_pointer>::type;

}} // namespace boost::python::detail

#endif // POINTEE_DWA2002323_HPP
