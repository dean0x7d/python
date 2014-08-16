#pragma once

#include <boost/python/detail/type_list.hpp>
#include <boost/python/cpp14/utility.hpp>

namespace boost { namespace python { namespace detail { 

namespace tl {
    template<class T, class I> 
    struct sub;
    
    template<class... Ts, std::size_t... Is>
    struct sub<type_list<Ts...>, cpp14::index_sequence<Is...>> {
        using tup = std::tuple<Ts...>;
        using type = type_list< typename std::tuple_element<Is, tup>::type... >;
    };

    template<class T1, class T2> 
    struct concat;
    
    template<class... Ts, class... Us>
    struct concat<type_list<Ts...>, type_list<Us...>> {
        using type = type_list<Ts..., Us...>;
    };

    template<class T, int N>
    struct get {
        using type = std::false_type;
    };
    
    template<int N, class T, class... Ts>
    struct get<type_list<T, Ts...>, N> {
        using type = typename std::tuple_element<N, std::tuple<T, Ts...>>::type;
    };
}

// Returns a type_list with only the first N elements
template<class List, int N>
using sub_t = typename tl::sub<List, cpp14::make_index_sequence<N>>::type;

// Returns a type_list without the last N elements
template<class List, int N>
using drop_t = sub_t<List, List::is_empty ? 0 : List::size - N>; 

// Returns a concatenation of the two type_lists
template<class List1, class List2>
using concat_t = typename tl::concat<List1, List2>::type;

// Returns the Nth type, or std::false_type if the list is empty
template<class List, int N>
using get_t = typename tl::get<List, N>::type;

template<class List>
using front_t = get_t<List, 0>;

template<class List>
using back_t = get_t<List, List::size - 1>;

}}} // namespace boost::python::detail
