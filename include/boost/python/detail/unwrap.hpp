#pragma once

namespace boost { namespace python { namespace detail {

// This struct is required for compatibility with GCC
template<class...>
struct void_type {
    using type = void;
};

template<class... Ts>
using void_t = typename void_type<Ts...>::type;

//
//  Return member T::type or Default if T::type doesn't exist.
//
template<class T, class Default = T, class = void>
struct unwrap {
    using type = Default;
};

template<class T, class Default>
struct unwrap<T, Default, void_t<typename T::type>> {
    using type = typename T::type;
};

template<class T, class Default = T>
using unwrap_t = typename unwrap<T, Default>::type;

}}} // namespace boost::python::detail
