// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef FORWARD_DWA20011215_HPP
# define FORWARD_DWA20011215_HPP

# include <functional>
# include <boost/python/detail/value_arg.hpp>
# include <boost/python/detail/copy_ctor_mutates_rhs.hpp>

namespace boost { namespace python { namespace objects { 

// Very much like boost::reference_wrapper<T>, except that in this
// case T can be a reference already without causing a
// reference-to-reference error.
template <class T>
struct reference_to_value
{
    using reference = cpp14::add_lvalue_reference_t<cpp14::add_const_t<T>>;
    
    reference_to_value(reference x) : m_value(x) {}
    reference get() const { return m_value; }
 private:
    reference m_value;
};

template<typename T> 
struct is_reference_wrapper {
    static constexpr bool value = false;
};

template<typename T> struct is_reference_wrapper< std::reference_wrapper<T> > {
    static constexpr bool value = true;
};

template<typename T> 
struct unwrap_reference {
    typedef T type;
};

template<typename T> 
struct unwrap_reference< std::reference_wrapper<T> > {
    typedef T type;
};


// A little metaprogram which selects the type to pass through an
// intermediate forwarding function when the destination argument type
// is T.
template<class T>
using forward = cpp14::conditional_t<
    python::detail::copy_ctor_mutates_rhs<T>::value || std::is_scalar<T>::value,
    T,
    reference_to_value<T>
>;

template<typename T>
struct unforward
{
    typedef typename unwrap_reference<T>::type& type;
};

template<typename T>
struct unforward<reference_to_value<T> >
{
    typedef T type;
};

template <typename T>
struct unforward_cref
{
    using type = python::detail::value_arg_t<typename unwrap_reference<T>::type>;
};

template<typename T>
struct unforward_cref<reference_to_value<T> >
  : cpp14::add_lvalue_reference_t<cpp14::add_const_t<T>>
{
};


template <class T>
typename reference_to_value<T>::reference
do_unforward(reference_to_value<T> const& x, int)
{
    return x.get();
}

template <class T>
typename std::reference_wrapper<T>::type&
do_unforward(std::reference_wrapper<T> const& x, int)
{
    return x.get();
}

template <class T>
T const& do_unforward(T const& x, ...)
{
    return x;
}

}}} // namespace boost::python::objects

#endif // FORWARD_DWA20011215_HPP
