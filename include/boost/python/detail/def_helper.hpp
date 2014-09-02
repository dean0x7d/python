// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef DEF_HELPER_DWA200287_HPP
# define DEF_HELPER_DWA200287_HPP

# include <boost/python/args.hpp>
# include <boost/python/detail/def_helper_fwd.hpp>

# include <boost/python/cpp14/type_traits.hpp>
# include <tuple>

namespace boost { namespace python {

struct default_call_policies;

namespace detail
{
  namespace impl {
    template<template<class> class Predicate, class Tuple>
    struct get_by_predicate;
    
    template<template<class> class Predicate, class... Args>
    struct get_by_predicate<Predicate, std::tuple<Args...>>
    {
        // Predicate result is 'true', return the index.
        template<bool result, int N, class... Tail>
        struct find_index {
            static constexpr auto value = N;
        };

        // Predicate result is 'false', test the next element.
        template<int N, class T, class... Tail>
        struct find_index<false, N, T, Tail...> {
            static constexpr auto value = find_index<
                Predicate<
                    cpp14::remove_cv_t<cpp14::remove_reference_t<T>>
                >::value, N + 1, Tail...
            >::value;
        };
    
        static constexpr auto index = find_index<false, -1, Args...>::value;
        
        using Tuple = std::tuple<Args...>;
        static_assert(index < std::tuple_size<Tuple>::value, 
                      "No tuple element satisfies the predicate");
        
        using type = typename std::tuple_element<index, Tuple>::type;
        static type extract(const Tuple& t) {
            return std::get<index>(t);
        }
    };
  }

  template<template<class> class Predicate, class Tuple>
  using get_by_predicate_t = typename impl::get_by_predicate<Predicate, Tuple>::type;

  // Get the first tuple element that satisfies the predicate.
  // References and cv qualifiers are removed from the element type 
  // before passing it to the predicate.
  template<template<class> class Predicate, class Tuple>
  get_by_predicate_t<Predicate, Tuple> get_by_predicate(const Tuple& t) {
      return impl::get_by_predicate<Predicate, Tuple>::extract(t);
  }

  //
  // A helper class for decoding the optional arguments to def()
  // invocations, which can be supplied in any order and are
  // discriminated by their type properties. The template parameters
  // are expected to be the types of the actual (optional) arguments
  // passed to def().
  //
  template <class T1, class T2, class T3, class T4>
  struct def_helper
  {
      // A tuple type which begins with references to the supplied
      // arguments and ends with actual representatives of the default
      // types.
      using all_t = std::tuple<
          T1 const&
          , T2 const&
          , T3 const&
          , T4 const&
          , default_call_policies
          , detail::keywords<0>
          , char const*
          , void(not_specified::*)()   // A function pointer type which is never an
                                       // appropriate default implementation
      >;

      // Constructors; these initialize an member of the tuple type
      // shown above.
      def_helper(T1 const& a1) : def_helper(a1,m_nil,m_nil,m_nil) {}
      def_helper(T1 const& a1, T2 const& a2) : def_helper(a1,a2,m_nil,m_nil) {}
      def_helper(T1 const& a1, T2 const& a2, T3 const& a3) : def_helper(a1,a2,a3,m_nil) {}
      def_helper(T1 const& a1, T2 const& a2, T3 const& a3, T4 const& a4) 
      : m_all(a1,a2,a3,a4,{},{},{},{}) {}

   private: // predicates
      template<class T>
      struct is_doc {
          static constexpr bool value = !(std::is_class<T>::value || 
                                          std::is_member_function_pointer<T>::value);
      };
      
      template<class T>
      struct is_policy {
          static constexpr bool value = (!std::is_same<not_specified, T>::value &&
                                         std::is_class<T>::value &&
                                         !is_keywords<T>::value);
      };
      
   public: // Constants which can be used for static assertions.

      // Users must not supply a default implementation for non-class
      // methods.
      using default_implementation_t = get_by_predicate_t<std::is_member_function_pointer, all_t>;
      static constexpr bool has_default_implementation = 
              !std::is_same<default_implementation_t, void(not_specified::*)()>::value;
      
   public: // Extractor functions which pull the appropriate value out
           // of the tuple
      char const* doc() const
      {
          return get_by_predicate<is_doc>(m_all);
      }
      
      get_by_predicate_t<is_keywords, all_t> keywords() const
      {
          return get_by_predicate<is_keywords>(m_all);
      }
      
      get_by_predicate_t<is_policy, all_t> policies() const
      {
          return get_by_predicate<is_policy>(m_all);
      }

      default_implementation_t default_implementation() const
      {
          return get_by_predicate<std::is_member_function_pointer>(m_all);
      }
      
   private: // data members
      all_t m_all; 
      not_specified m_nil; // for filling in not_specified slots
  };
}

}} // namespace boost::python::detail

#endif // DEF_HELPER_DWA200287_HPP
