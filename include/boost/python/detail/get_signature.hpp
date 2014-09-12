#ifndef BOOST_PYTHON_FN_CC
#error Do not include this file
#else

// as 'get_signature(RT(*)(Ts...), void*)' is the same
// function as 'get_signature(RT(__cdecl *)(Ts...), void*)',
// we don't define it twice
#if !defined(BOOST_PYTHON_FN_CC_IS_CDECL)

template <class RT, class... Ts>
inline type_list<RT, Ts...> 
get_signature(RT(BOOST_PYTHON_FN_CC *)(Ts...), void* = nullptr)
{
    return {};
}

#endif


template <class RT, class ClassT, class... Ts, class Target = ClassT>
inline type_list<RT, most_derived_t<Target, ClassT>&, Ts...>
get_signature(RT(BOOST_PYTHON_FN_CC ClassT::*)(Ts...), Target* = nullptr)
{
    return {};
}

template <class RT, class ClassT, class... Ts, class Target = ClassT>
inline type_list<RT, most_derived_t<Target, ClassT>&, Ts...>
get_signature(RT(BOOST_PYTHON_FN_CC ClassT::*)(Ts...) const, Target* = nullptr)
{
    return {};
}

template <class RT, class ClassT, class... Ts, class Target = ClassT>
inline type_list<RT, most_derived_t<Target, ClassT>&, Ts...>
get_signature(RT(BOOST_PYTHON_FN_CC ClassT::*)(Ts...) volatile, Target* = nullptr)
{
    return {};
}

template <class RT, class ClassT, class... Ts, class Target = ClassT>
inline type_list<RT, most_derived_t<Target, ClassT>&, Ts...>
get_signature(RT(BOOST_PYTHON_FN_CC ClassT::*)(Ts...) const volatile, Target* = nullptr)
{
    return {};
}

#endif // BOOST_PYTHON_FN_CC
