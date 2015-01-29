// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef MAKE_FUNCTION_DWA20011221_HPP
# define MAKE_FUNCTION_DWA20011221_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/default_call_policies.hpp>
# include <boost/python/args.hpp>
# include <boost/python/detail/caller.hpp>

# include <boost/python/object/function_object.hpp>

namespace boost { namespace python {

namespace detail
{
  // make_function_aux --
  //
  // These helper functions for make_function (below) do the raw work
  // of constructing a Python object from some invokable entity. See
  // <boost/python/detail/caller.hpp> for more information about how
  // the Sig arguments is used.
  template <class F, class CallPolicies, class Sig>
  object make_function_aux(
      F f                               // An object that can be invoked by detail::invoke()
      , CallPolicies const& p           // CallPolicies to use in the invocation
      , Sig const&                      // An MPL sequence of argument types expected by F
      )
  {
      return objects::function_object(
          detail::caller<F,CallPolicies,Sig>(f)
      );
  }

  // As above, except that it accepts argument keywords. NumKeywords
  // is used only for a compile-time assertion to make sure the user
  // doesn't pass more keywords than the function can accept. To
  // disable all checking, pass std::integral_constant<int, 0> for NumKeywords.
  template <class F, class CallPolicies, class Sig, class NumKeywords>
  object make_function_aux(
      F f
      , CallPolicies const& p
      , Sig const&
      , detail::keyword_range const& kw // a [begin,end) pair of iterators over keyword names
      , NumKeywords                     // An MPL integral type wrapper: the size of kw
      )
  {
      static_assert(NumKeywords::value <= Sig::size - 1, "More keywords than function arguments");
    
      return objects::function_object(
          detail::caller<F,CallPolicies,Sig>(f)
        , kw);
  }

  //   Helpers for make_function when called with 3 arguments.  These
  //   dispatch functions are used to discriminate between the cases
  //   when the 3rd argument is keywords or when it is a signature.
  //
  // @group {
  template <class F, class CallPolicies, class Keywords>
  object make_function_dispatch(F f, CallPolicies const& policies, 
                                Keywords const& kw, std::true_type)
  {
      return detail::make_function_aux(
          f
        , policies
        , detail::get_signature(f)
        , kw.range()
        , std::integral_constant<int, Keywords::size>()
      );
  }

  template <class F, class CallPolicies, class Signature>
  object make_function_dispatch(F f, CallPolicies const& policies, 
                                Signature const& sig, std::false_type)
  {
      return detail::make_function_aux(
          f
        , policies
        , sig
      );
  }
  // }
  
 }

//   These overloaded functions wrap a function or member function
//   pointer as a Python object, using optional CallPolicies,
//   Keywords, and/or Signature.
//
template <class F>
object make_function(F f)
{
    return detail::make_function_aux(
        f,default_call_policies(), detail::get_signature(f));
}

template <class F, class CallPolicies>
object make_function(F f, CallPolicies const& policies)
{
    return detail::make_function_aux(
        f, policies, detail::get_signature(f));
}

template <class F, class CallPolicies, class KeywordsOrSignature>
object make_function(
    F f
  , CallPolicies const& policies
  , KeywordsOrSignature const& keywords_or_signature)
{
    return detail::make_function_dispatch(
        f
      , policies
      , keywords_or_signature
      , detail::is_keywords<KeywordsOrSignature>{}
    );
}

template <class F, class CallPolicies, class Keywords, class Signature>
object make_function(
    F f
  , CallPolicies const& policies
  , Keywords const& kw
  , Signature const& sig
 )
{
    return detail::make_function_aux(
          f
        , policies
        , sig
        , kw.range()
        , std::integral_constant<int, Keywords::size>()
      );
}

}} 


#endif // MAKE_FUNCTION_DWA20011221_HPP
