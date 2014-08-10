#pragma once
#include <type_traits>

namespace cpp14 {
    
template<typename T>
using remove_cv_t = typename std::remove_cv<T>::type;
    
template<typename T>
using add_cv_t = typename std::add_cv<T>::type;

}
