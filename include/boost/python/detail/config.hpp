//  (C) Copyright David Abrahams 2000.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  The author gratefully acknowleges the support of Dragon Systems, Inc., in
//  producing this work.

#ifndef CONFIG_DWA052200_H_
# define CONFIG_DWA052200_H_

# include <boost/config.hpp>

# if defined(BOOST_MSVC)

#  pragma warning (disable : 4786) // disable truncated debug symbols
#  pragma warning (disable : 4251) // disable exported dll function
#  pragma warning (disable : 4800) //'int' : forcing value to bool 'true' or 'false'
#  pragma warning (disable : 4275) // non dll-interface class

# endif

/*****************************************************************************
 *
 *  Set up dll import/export options:
 *
 ****************************************************************************/

// backwards compatibility:
#ifdef BOOST_PYTHON_STATIC_LIB
#  define BOOST_PYTHON_STATIC_LINK
# elif !defined(BOOST_PYTHON_DYNAMIC_LIB)
#  define BOOST_PYTHON_DYNAMIC_LIB
#endif

#if defined(BOOST_PYTHON_DYNAMIC_LIB)

#  ifdef __GNUC__
#    define BOOST_PYTHON_USE_GCC_SYMBOL_VISIBILITY 1
#  endif 

#  if BOOST_PYTHON_USE_GCC_SYMBOL_VISIBILITY
#     if defined(BOOST_PYTHON_SOURCE)
#        define BOOST_PYTHON_DECL __attribute__ ((__visibility__("default")))
#        define BOOST_PYTHON_BUILD_DLL
#     else
#        define BOOST_PYTHON_DECL
#     endif
#     define BOOST_PYTHON_DECL_FORWARD
#     define BOOST_PYTHON_DECL_EXCEPTION __attribute__ ((__visibility__("default")))
#  elif (defined(_WIN32) || defined(__CYGWIN__))
#     if defined(BOOST_PYTHON_SOURCE)
#        define BOOST_PYTHON_DECL __declspec(dllexport)
#        define BOOST_PYTHON_BUILD_DLL
#     else
#        define BOOST_PYTHON_DECL __declspec(dllimport)
#     endif
#  endif

#endif

#ifndef BOOST_PYTHON_DECL
#  define BOOST_PYTHON_DECL
#endif

#ifndef BOOST_PYTHON_DECL_FORWARD
#  define BOOST_PYTHON_DECL_FORWARD BOOST_PYTHON_DECL
#endif

#ifndef BOOST_PYTHON_DECL_EXCEPTION
#  define BOOST_PYTHON_DECL_EXCEPTION BOOST_PYTHON_DECL
#endif

//  enable automatic library variant selection  ------------------------------// 

#if !defined(BOOST_PYTHON_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_PYTHON_NO_LIB)
//
// Set the name of our library, this will get undef'ed by auto_link.hpp
// once it's done with it:
//
#define BOOST_LIB_NAME boost_python
//
// If we're importing code from a dll, then tell auto_link.hpp about it:
//
#ifdef BOOST_PYTHON_DYNAMIC_LIB
#  define BOOST_DYN_LINK
#endif
//
// And include the header that does the work:
//
#include <boost/config/auto_link.hpp>
#endif  // auto-linking disabled

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
#define BOOST_PYTHON_SUPPORTS_PY_SIGNATURES // enables smooth transition
#endif

#endif // CONFIG_DWA052200_H_
