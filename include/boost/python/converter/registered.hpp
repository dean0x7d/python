// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef REGISTERED_DWA2002710_HPP
# define REGISTERED_DWA2002710_HPP
# include <boost/python/converter/registry.hpp>
# include <boost/python/converter/registrations.hpp>
# include <boost/python/cpp14/type_traits.hpp>

# include <boost/shared_ptr.hpp>

namespace boost { namespace python { namespace converter {

namespace detail
{
  template <class T>
  struct registered_base
  {
      static registration const& converters;
  };
}

template <class T>
using registered = detail::registered_base<
    cpp14::remove_cv_t<
        cpp14::remove_reference_t<T>
    >
>;

template <class T>
using registered_pointee = detail::registered_base<
    cpp14::remove_pointer_t<
        cpp14::remove_reference_t<T>
    >
>;

//
// implementations
//
namespace detail
{
  template<class T>
  struct lookup_helper {
      static registration const& execute() {
          return registry::lookup(type_id<T>());
      }
  };

  template<class T>
  struct lookup_helper<shared_ptr<T>> {
      static registration const& execute() {
          return registry::lookup_shared_ptr(type_id<shared_ptr<T>>());
      }
  };

  template <class T>
  registration const& registered_base<T>::converters = detail::lookup_helper<T>::execute();
}

}}} // namespace boost::python::converter

#endif // REGISTERED_DWA2002710_HPP
