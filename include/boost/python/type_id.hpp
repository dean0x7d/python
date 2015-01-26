// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_ID_DWA2002517_HPP
# define TYPE_ID_DWA2002517_HPP

# include <boost/python/detail/prefix.hpp>
# include <typeindex>
# include <cstring>
# include <ostream>

# if !defined(BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE) && defined(__GNUC__) && !defined(__EDG_VERSION__)
#  define BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
namespace boost { namespace python { namespace detail {
    BOOST_PYTHON_DECL char const* gcc_demangle(char const*);
}}}
# endif

namespace boost { namespace python { 

// Type ids which represent the same information as std::type_index
// (i.e. the top-level reference and cv-qualifiers are stripped), but
// which have demangled names if supported.
// -> This should work across shared libraries on C++11 compilers.
struct type_info : std::type_index {
    using std::type_index::type_index;

    char const* name() const {
#  ifdef BOOST_PYTHON_HAVE_GCC_CP_DEMANGLE
        return detail::gcc_demangle(std::type_index::name());
#  else
        return std::type_index::name();
#  endif
    }

    friend BOOST_PYTHON_DECL std::ostream& operator<<(std::ostream&, type_info const&);
};

template <class T>
inline type_info type_id() { return {typeid(T)}; }

}} // namespace boost::python

#endif // TYPE_ID_DWA2002517_HPP
