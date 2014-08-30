// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef REGISTERED_POINTEE_DWA2002710_HPP
# define REGISTERED_POINTEE_DWA2002710_HPP
# include <boost/python/converter/registered.hpp>
# include <boost/python/converter/registry.hpp>
# include <boost/python/cpp14/type_traits.hpp>

namespace boost { namespace python { namespace converter { 

struct registration;

template <class T>
struct registered_pointee
    : registered<
          cpp14::remove_pointer_t<
              cpp14::remove_cv_t<
                  cpp14::remove_reference_t<T>
              >
          >
      >
{
};

}}} // namespace boost::python::converter

#endif // REGISTERED_POINTEE_DWA2002710_HPP
