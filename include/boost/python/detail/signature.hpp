// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIGNATURE_DWA20021121_HPP
# define SIGNATURE_DWA20021121_HPP

# include <boost/python/type_id.hpp>
# include <boost/python/detail/type_list.hpp>

# include <memory>

namespace boost { namespace python { namespace detail { 

struct signature_element {
    char const* basename;
    PyTypeObject const* pytype;
    bool lvalue;
};

using py_func_sig_info = std::unique_ptr<signature_element[]>;

template<class T>
using is_reference_to_non_const = std::integral_constant<bool,
	std::is_reference<T>::value && !std::is_const<cpp14::remove_reference_t<T>>::value
>;

inline PyTypeObject const* get_expected_from_python_type(type_info ti) {
    auto r = converter::registry::query(ti);
    return r ? r->expected_from_python_type() : nullptr;
};

template<class Sig> struct signature;

template<class... Args>
struct signature<type_list<Args...>> {
    static py_func_sig_info elements() {
        return std::unique_ptr<signature_element[]>{new signature_element[sizeof...(Args)]{
            { type_id<Args>().name(),
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
              get_expected_from_python_type(type_id<cpp14::remove_pointer_t<Args>>()),
#else
              nullptr,
#endif
              is_reference_to_non_const<Args>::value }...
        }};
    }
};

}}} // namespace boost::python::detail

#endif // SIGNATURE_DWA20021121_HPP
