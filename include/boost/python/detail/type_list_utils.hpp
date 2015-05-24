#pragma once
#include <boost/python/detail/type_list.hpp>

namespace boost { namespace python { namespace detail { namespace tl {

struct invalid_type {};

namespace impl {
    template<class List, std::size_t N, class...>
    struct sub;

    template<std::size_t N, class... Us>
    struct sub<type_list<>, N, Us...> {
        using type = type_list<Us...>;
    };

    template<class T, class... Tail, class... Us>
    struct sub<type_list<T, Tail...>, 0, Us...> {
        using type = type_list<Us...>;
    };

    template<std::size_t N, class T, class... Tail, class... Us>
    struct sub<type_list<T, Tail...>, N, Us...> {
        using type = typename sub<type_list<Tail...>, N - 1, Us..., T>::type;
    };
}

// Return the first N elements
template<class List, std::size_t N>
using sub_t = typename impl::sub<List, N>::type;

// Drop the last N elements from the list
template<class List, std::size_t N>
using drop_t = sub_t<List, List::is_empty ? 0 : List::size - N>;


namespace impl {
    template<class List1, class List2>
    struct concat;
    
    template<class... Ts, class... Us>
    struct concat<type_list<Ts...>, type_list<Us...>> {
        using type = type_list<Ts..., Us...>;
    };
}

// Concatenate two type_lists
template<class List1, class List2>
using concat_t = typename impl::concat<List1, List2>::type;


namespace impl {
    template<class List, std::size_t N>
    struct get {
        using type = invalid_type;
    };

    template<class T, class... Tail>
    struct get<type_list<T, Tail...>, 0> {
        using type = T;
    };

    template<std::size_t N, class T, class... Tail>
    struct get<type_list<T, Tail...>, N> {
        using type = typename get<type_list<Tail...>, N - 1>::type;
    };
}

// Get the Nth type, or invalid_type if the list is empty
template<class List, std::size_t N>
using get_t = typename impl::get<List, N>::type;

template<class List>
using front_t = get_t<List, 0>;

template<class List>
using back_t = get_t<List, List::is_empty ? 0 : List::size - 1>;

}}}} // namespace boost::python::detail::tl
