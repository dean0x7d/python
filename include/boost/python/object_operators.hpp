// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef OBJECT_OPERATORS_DWA2002617_HPP
# define OBJECT_OPERATORS_DWA2002617_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/object_core.hpp>
# include <boost/python/cpp14/type_traits.hpp>
# include <boost/python/detail/is_xxx.hpp>

namespace boost { namespace python { namespace api {

template <class L, class R = L>
struct is_object_operators {
    static constexpr bool value = python::detail::is_base_template_of<object_operators, L>::value
                               || python::detail::is_base_template_of<object_operators, R>::value;
    using type = std::integral_constant<bool, value>;
};

template <class L, class R, class T>
using enable_binary_t = cpp14::enable_if_t<is_object_operators<L,R>::value, T>;

template <class U>
inline object_operators<U>::operator bool_type() const {
    int is_true = PyObject_IsTrue(get_managed_object(this->derived()));
    if (is_true < 0) throw_error_already_set();
    return is_true ? &object::ptr : nullptr;
}

# define BOOST_PYTHON_BINARY_OPERATOR(op)                               \
BOOST_PYTHON_DECL object operator op(object const& l, object const& r); \
template <class L, class R>                                             \
enable_binary_t<L, R, object> operator op(L const& l, R const& r)       \
{                                                                       \
    return object(l) op object(r);                                      \
}
BOOST_PYTHON_BINARY_OPERATOR(>)
BOOST_PYTHON_BINARY_OPERATOR(>=)
BOOST_PYTHON_BINARY_OPERATOR(<)
BOOST_PYTHON_BINARY_OPERATOR(<=)
BOOST_PYTHON_BINARY_OPERATOR(==)
BOOST_PYTHON_BINARY_OPERATOR(!=)
BOOST_PYTHON_BINARY_OPERATOR(+)
BOOST_PYTHON_BINARY_OPERATOR(-)
BOOST_PYTHON_BINARY_OPERATOR(*)
BOOST_PYTHON_BINARY_OPERATOR(/)
BOOST_PYTHON_BINARY_OPERATOR(%)
BOOST_PYTHON_BINARY_OPERATOR(<<)
BOOST_PYTHON_BINARY_OPERATOR(>>)
BOOST_PYTHON_BINARY_OPERATOR(&)
BOOST_PYTHON_BINARY_OPERATOR(^)
BOOST_PYTHON_BINARY_OPERATOR(|)
# undef BOOST_PYTHON_BINARY_OPERATOR

        
# define BOOST_PYTHON_INPLACE_OPERATOR(op)                              \
BOOST_PYTHON_DECL object& operator op(object& l, object const& r);      \
template <class R>                                                      \
object& operator op(object& l, R const& r)                              \
{                                                                       \
    return l op object(r);                                              \
}
BOOST_PYTHON_INPLACE_OPERATOR(+=)
BOOST_PYTHON_INPLACE_OPERATOR(-=)
BOOST_PYTHON_INPLACE_OPERATOR(*=)
BOOST_PYTHON_INPLACE_OPERATOR(/=)
BOOST_PYTHON_INPLACE_OPERATOR(%=)
BOOST_PYTHON_INPLACE_OPERATOR(<<=)
BOOST_PYTHON_INPLACE_OPERATOR(>>=)
BOOST_PYTHON_INPLACE_OPERATOR(&=)
BOOST_PYTHON_INPLACE_OPERATOR(^=)
BOOST_PYTHON_INPLACE_OPERATOR(|=)
# undef BOOST_PYTHON_INPLACE_OPERATOR

}}} // namespace boost::python

#endif // OBJECT_OPERATORS_DWA2002617_HPP
