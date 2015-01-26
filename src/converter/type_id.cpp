// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/python/type_id.hpp>
#include <vector>
#include <algorithm>

#ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
# include <cxxabi.h>
#endif 

namespace boost { namespace python {

#ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE

namespace
{
  struct compare_first_cstring
  {
      template <class T>
      bool operator()(T const& x, T const& y)
      {
          return std::strcmp(x.first,y.first) < 0;
      }
  };
  
  struct free_mem
  {
      free_mem(char*p)
          : p(p) {}
    
      ~free_mem()
      {
          std::free(p);
      }
      char* p;
  };
}

namespace detail
{
  BOOST_PYTHON_DECL char const* gcc_demangle(char const* mangled)
  {
      static std::vector<std::pair<char const*, char const*>> demangler;
      
      auto p = std::lower_bound(
          demangler.begin(), demangler.end(),
          std::make_pair(mangled, (char const*)nullptr), compare_first_cstring()
      );

      if (p == demangler.end() || strcmp(p->first, mangled))
      {
          int status;
          free_mem keeper(::abi::__cxa_demangle(mangled, 0, 0, &status));
    
          assert(status != -3); // invalid argument error
    
          if (status == -1)
          {
              throw std::bad_alloc();
          }
          else
          {
              char const* demangled
                = status == -2
                  // Invalid mangled name.  Best we can do is to
                  // return it intact.
                  ? mangled
                  : keeper.p;

              p = demangler.insert(p, std::make_pair(mangled, demangled));
              keeper.p = 0;
          }
      }
      
      return p->second;
  }
}
#endif // BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE

BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream& os, type_info const& x)
{
    return os << x.name();
}

}} // namespace boost::python
