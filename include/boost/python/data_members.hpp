// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef DATA_MEMBERS_DWA2002328_HPP
# define DATA_MEMBERS_DWA2002328_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/handle.hpp>

# include <boost/python/return_value_policy.hpp>
# include <boost/python/return_by_value.hpp>
# include <boost/python/return_internal_reference.hpp>
# include <boost/python/make_function.hpp>

# include <boost/python/converter/builtin_converters.hpp>

# include <boost/python/detail/not_specified.hpp>
# include <boost/python/detail/value_arg.hpp>

namespace boost { namespace python { 

//
// This file defines the make_getter and make_setter function
// families, which are responsible for turning pointers, references,
// and pointers-to-data-members into callable Python objects which
// can be used for attribute access on wrapped classes.
//

namespace detail
{

  // A small function object which handles the getting and setting of
  // data members.
  template <class Data, class Class>
  struct member
  {
   public:      
      member(Data Class::*which) : m_which(which) {}
      
      Data& operator()(Class& c) const
      {
          return c.*m_which;
      }

      void operator()(Class& c, value_arg_t<Data> d) const
      {
          c.*m_which = d;
      }
   private:
      Data Class::*m_which;
  };

  // A small function object which handles the getting and setting of
  // non-member objects.
  template <class Data>
  struct datum
  {
   public:      
      datum(Data *which) : m_which(which) {}
      
      Data& operator()() const
      {
          return *m_which;
      }

      void operator()(value_arg_t<Data> d) const
      {
          *m_which = d;
      }
   private:
      Data *m_which;
  };
  
  //
  // Helper metafunction for determining the default CallPolicy to use
  // for attribute access.  If T is a [reference to a] class type X
  // whose conversion to python would normally produce a new copy of X
  // in a wrapped X class instance (as opposed to types such as
  // std::string, which are converted to native Python types, and
  // smart pointer types which produce a wrapped class instance of the
  // pointee type), to-python conversions will attempt to produce an
  // object which refers to the original C++ object, rather than a
  // copy.  See default_member_getter_policy for rationale.
  // 
  template <class T>
  using default_getter_by_ref = std::integral_constant<bool,
      to_python_value<value_arg_t<T>>::uses_registry &&
      std::is_class<cpp14::remove_reference_t<value_arg_t<T>>>::value
  >;

  // Metafunction computing the default CallPolicy to use for reading
  // data members
  //
  // If it's a regular class type (not an object manager or other
  // type for which we have to_python specializations, use
  // return_internal_reference so that we can do things like
  //    x.y.z =  1
  // and get the right result.
  template <class T>
  using default_member_getter_policy_t = cpp14::conditional_t<
      default_getter_by_ref<T>::value,
      return_internal_reference<>,
      return_value_policy<return_by_value>
  >;

  // Metafunction computing the default CallPolicy to use for reading
  // non-member data.
  template <class T>
  using default_datum_getter_policy_t = cpp14::conditional_t<
      default_getter_by_ref<T>::value,
      return_value_policy<reference_existing_object>,
      return_value_policy<return_by_value>
  >;

  //
  // make_getter helper function family -- These helpers to
  // boost::python::make_getter are used to dispatch behavior.

  // Handle non-member pointers with policies
  template <class D, class Policies>
  inline object make_getter(D* d, Policies const& policies)
  {
      return python::make_function(
          detail::datum<D>(d), policies, detail::type_list<D&>()
      );
  }
  
  // Handle non-member pointers without policies
  template <class D>
  inline object make_getter(D* d, not_specified)
  {
      return detail::make_getter(d, default_datum_getter_policy_t<D>());
  }

  // Handle pointers-to-members with policies
  template <class C, class D, class Policies>
  inline object make_getter(D C::*pm, Policies const& policies)
  {
      typedef C Class;
      return python::make_function(
          detail::member<D,Class>(pm)
        , policies
        , detail::type_list<D&,Class&>()
      );
  }
      
  // Handle pointers-to-members without policies
  template <class C, class D>
  inline object make_getter(D C::*pm, not_specified)
  {
      return detail::make_getter(pm, default_member_getter_policy_t<D>());
  }

  // Handle references
  template <class D, class P>
  inline object make_getter(D& d, P const& p)
  {
      // Just dispatch to the handler for pointer types.
      return detail::make_getter(&d, p);
  }

  //
  // make_setter helper function family -- These helpers to
  // boost::python::make_setter are used to dispatch behavior.
  
  // Handle non-member pointers
  template <class D, class Policies>
  inline object make_setter(D* p, Policies const& policies)
  {
      return python::make_function(
          detail::datum<D>(p), policies, detail::type_list<void,D const&>()
      );
  }

  // Handle pointers-to-members
  template <class C, class D, class Policies>
  inline object make_setter(D C::*pm, Policies const& policies)
  {
      return python::make_function(
          detail::member<D,C>(pm)
        , policies
        , detail::type_list<void, C&, D const&>()
      );
  }

  // Handle references
  template <class D, class Policies>
  inline object make_setter(D& x, Policies const& policies)
  {
      return detail::make_setter(&x, policies);
  }
}

//
// make_getter function family -- build a callable object which
// retrieves data through the first argument and is appropriate for
// use as the `get' function in Python properties .  The second,
// policies argument, is optional.
template <class D, class Policies = detail::not_specified>
inline object make_getter(D&& d, Policies policies = {})
{
    return detail::make_getter(std::forward<D>(d), std::forward<Policies>(policies));
}

//
// make_setter function family -- build a callable object which
// writes data through the first argument and is appropriate for
// use as the `set' function in Python properties .  The second,
// policies argument, is optional.
template <class D, class Policies = default_call_policies>
inline object make_setter(D&& d, Policies policies = {})
{
    return detail::make_setter(std::forward<D>(d), std::forward<Policies>(policies));
}

}} // namespace boost::python

#endif // DATA_MEMBERS_DWA2002328_HPP
