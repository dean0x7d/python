// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TUPLE_20020706_HPP
#define TUPLE_20020706_HPP

# include <boost/python/detail/prefix.hpp>

#include <boost/python/object.hpp>
#include <boost/python/converter/pytype_object_mgr_traits.hpp>

namespace boost { namespace python {

namespace detail
{
  struct BOOST_PYTHON_DECL tuple_base : object
  {
   protected:
      tuple_base();
      tuple_base(object_cref sequence);
      
      BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(tuple_base, object)

   private:
      static detail::new_reference call(object const&);
  };
}

class tuple : public detail::tuple_base
{
    using base = detail::tuple_base;
 public:
    tuple() {}

    template <class T>
    explicit tuple(T const& sequence)
        : base(object(sequence))
    {
    }

 public: // implementation detail -- for internal use only
    BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(tuple, base)
};

//
// Converter Specializations    // $$$ JDG $$$ moved here to prevent
//                              // G++ bug complaining specialization
                                // provided after instantiation
namespace converter
{
  template <>
  struct object_manager_traits<tuple>
      : pytype_object_manager_traits<&PyTuple_Type,tuple>
  {
  };
}


namespace detail 
{
  inline void tuple_set_items(tuple const& t, int n) {}

  template <class A, class... Args>
  inline void tuple_set_items(tuple const& t, int n, A const& a, Args const&... args)
  {
      PyTuple_SET_ITEM(t.ptr(), n, python::incref(python::object(a).ptr()));
      tuple_set_items(t, n + 1, args...);
  }
}

template <class... Args>
tuple make_tuple(Args const&... args)
{
    tuple result((detail::new_reference)::PyTuple_New(sizeof...(Args)));
    detail::tuple_set_items(result, 0, args...);
    return result;
}

}}  // namespace boost::python

#endif

