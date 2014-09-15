///////////////////////////////////////////////////////////////////////////////
//
// Copyright David Abrahams 2002, Joel de Guzman, 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
///////////////////////////////////////////////////////////////////////////////
#ifndef DEFAULTS_GEN_JDG20020807_HPP
#define DEFAULTS_GEN_JDG20020807_HPP

#include <boost/python/args_fwd.hpp>
#include <boost/python/default_call_policies.hpp>
#include <boost/python/detail/type_list_utils.hpp>

namespace boost { namespace python {

namespace detail
{
  // overloads_base is used as a base class for all function
  // stubs. This class holds the doc_string of the stubs.
  struct overloads_base
  {
      overloads_base(char const* doc_)
          : m_doc(doc_) {}

      overloads_base(char const* doc_, detail::keyword_range const& kw)
          : m_doc(doc_), m_keywords(kw) {}

      char const* doc_string() const
      {
          return m_doc;
      }

      detail::keyword_range const& keywords() const
      {
          return m_keywords;
      }

   private:
      char const* m_doc;
      detail::keyword_range m_keywords;
  };

  // overloads_proxy is generated by the overloads_common operator[] (see
  // below). This class holds a user defined call policies of the stubs.
  template <class CallPoliciesT, class OverloadsT>
  struct overloads_proxy
      : public overloads_base
  {
      using type = OverloadsT;

      overloads_proxy(
          CallPoliciesT const& policies_
          , char const* doc
          , keyword_range const& kw
          )
          : overloads_base(doc, kw)
            , policies(policies_)
      {}

      CallPoliciesT
      call_policies() const
      {
          return policies;
      }

      CallPoliciesT policies;
  };

  // overloads_common is our default function stubs base class. This
  // class returns the default_call_policies in its call_policies()
  // member function.  It can generate a overloads_proxy however through
  // its operator[]
  template <class DerivedT>
  struct overloads_common
      : public overloads_base
  {
      overloads_common(char const* doc)
          : overloads_base(doc) {}

      overloads_common(char const* doc, keyword_range const& kw)
          : overloads_base(doc, kw) {}

      default_call_policies
      call_policies() const
      {
          return default_call_policies();
      }

      template <class CallPoliciesT>
      overloads_proxy<CallPoliciesT, DerivedT>
      operator[](CallPoliciesT const& policies) const
      {
          return overloads_proxy<CallPoliciesT, DerivedT>(
              policies, this->doc_string(), this->keywords());
      }
  };

  template<template<class> class Generator, int min, int max, bool is_class = false>
  struct overloads : overloads_common<overloads<Generator, min, max, is_class>>
  {
      using type = overloads<Generator, min, max, is_class>;
      using base = overloads_common<type>;
      
      static constexpr int n_funcs = max - min + 1;
      static constexpr int max_args = n_funcs + is_class;

      template<class Sig>
      using gen = Generator<Sig>;

      overloads(char const *doc = 0)
          : base(doc) {}
      
      template<std::size_t N>
      overloads(char const *doc, ::boost::python::detail::keywords<N> const &kw)
          : base(doc, kw.range())
      {
          static_assert(N <= max + is_class, "More keywords than function arguments");
      }
      
      template<std::size_t N>
      overloads(::boost::python::detail::keywords<N> const &kw, char const *doc = 0)
          : base(doc, kw.range())
      {
          static_assert(N <= max + is_class, "More keywords than function arguments");
      }
  };
}}} // namespace boost::python::detail


///////////////////////////////////////////////////////////////////////////////
//
//      Given generator_name, fname, min_args and max_args, These macros
//      generate function stubs that forward to a function or member function
//      named fname. max_args is the arity of the function or member function
//      fname. fname can have default arguments. min_args is the minimum
//      arity that fname can accept.
//
//      There are two versions:
//
//          1. BOOST_PYTHON_FUNCTION_OVERLOADS for free functions
//          2. BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS for member functions.
//
///////////////////////////////////////////////////////////////////////////////

#define BOOST_PYTHON_FUNCTION_OVERLOADS(name, function_name, min, max)                      \
template<class Sig> struct gen_##name;                                                      \
                                                                                            \
template<class Return, class... Args>                                                       \
struct gen_##name<::boost::python::detail::type_list<Return, Args...>>                      \
{                                                                                           \
    static Return func(Args... args) {                                                      \
        return function_name(args...);                                                      \
    }                                                                                       \
};                                                                                          \
                                                                                            \
using name = ::boost::python::detail::overloads<gen_##name, min, max>;

#define BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(name, function_name, min, max)               \
template<class Sig> struct gen_##name;                                                      \
                                                                                            \
template<class Return, class Class, class... Args>                                          \
struct gen_##name<::boost::python::detail::type_list<Return, Class, Args...>>               \
{                                                                                           \
    static Return func(Class&& c, Args... args) {                                           \
        return std::forward<Class>(c).function_name(args...);                               \
    }                                                                                       \
};                                                                                          \
                                                                                            \
using name = ::boost::python::detail::overloads<gen_##name, min, max, true>;

///////////////////////////////////////////////////////////////////////////////
#endif // DEFAULTS_GEN_JDG20020807_HPP


