// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/bases.hpp>
#include <type_traits>

struct A;
struct B;

template <class X, class Y, class Z>
using choose_bases_t = boost::python::detail::select_bases_t<X, 
                          boost::python::detail::select_bases_t<Y,
                              boost::python::detail::select_bases_t<Z>>>;

int main()
{
    static_assert(boost::python::detail::specifies_bases<
                  boost::python::bases<A,B> >::value, "");

    static_assert(!boost::python::detail::specifies_bases<
                  boost::python::bases<A,B>& >::value, "");

    static_assert(!boost::python::detail::specifies_bases<
                  void* >::value, "");

    static_assert(!boost::python::detail::specifies_bases<
                  int >::value, "");

    static_assert(!boost::python::detail::specifies_bases<
                  int[5] >::value, "");

    using collected1 = boost::python::detail::select_bases_t<int,
                           boost::python::detail::select_bases_t<char*>>;

    static_assert(std::is_same<collected1,boost::python::bases<> >::value, "");
    static_assert(std::is_same<choose_bases_t<int,char*,long>, boost::python::bases<> >::value, "");
    
    using collected2 = boost::python::detail::select_bases_t<int,
                           boost::python::detail::select_bases_t<boost::python::bases<A,B>,
                               boost::python::detail::select_bases_t<A>>>;

    static_assert(std::is_same<collected2,boost::python::bases<A,B> >::value, "");
    static_assert(std::is_same<choose_bases_t<int,boost::python::bases<A,B>,long>, boost::python::bases<A,B> >::value, "");
}
