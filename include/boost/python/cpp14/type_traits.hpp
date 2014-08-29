#pragma once
#include <type_traits>

namespace cpp14 {

template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;
template<typename T>
using add_cv_t = typename std::add_cv<T>::type;
template<typename T>
using remove_const_t = typename std::remove_const<T>::type;
template<typename T>
using add_const_t = typename std::add_const<T>::type;
template<typename T>
using remove_volatile_t = typename std::remove_volatile<T>::type;
template<typename T>
using add_volatile_t = typename std::add_volatile<T>::type;

template< class T >
using remove_reference_t = typename std::remove_reference<T>::type;
template< class T >
using add_lvalue_reference_t = typename std::add_lvalue_reference<T>::type;
template< class T >
using add_rvalue_reference = typename std::add_rvalue_reference<T>::type;

template<typename T>
using remove_pointer_t = typename std::remove_pointer<T>::type;
template<typename T>
using add_pointer_t = typename std::add_pointer<T>::type;
    
template<bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

}
