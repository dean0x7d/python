// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/python/type_id.hpp>
#include <unordered_map>
#include <memory>

#ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
# include <cxxabi.h>
#endif 

namespace boost { namespace python {

#ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
namespace detail
{
  BOOST_PYTHON_DECL char const* gcc_demangle(char const* mangled)
  {
      // Note: the memory allocated for the demangled names is freed by the OS.
      static std::unordered_map<char const*, char const*> type_names;

      auto& demangled = type_names[mangled];
      if (demangled == nullptr) {
          int status = -4;
          std::unique_ptr<char, void(*)(void*)> p{
              abi::__cxa_demangle(mangled, nullptr, nullptr, &status),
              std::free
          };
          assert(status != -3); // invalid argument error
    
          if (status == -1) {
              throw std::bad_alloc();
          }
          else {
              // In case of an invalid mangled name, the best we can do is to return it intact.
              demangled = (status == -2) ? mangled : p.release();
          }
      }
      
      return demangled;
  }
}
#endif // BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE

BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, type_info const& x)
{
    return os << x.name();
}

}} // namespace boost::python
