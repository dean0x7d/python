// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SIGNATURE_DWA20021121_HPP
# define SIGNATURE_DWA20021121_HPP

# include <boost/python/type_id.hpp>
# include <boost/python/detail/type_list.hpp>
# include <boost/python/detail/indirect_traits.hpp>
# include <boost/python/converter/pytype_function.hpp>

namespace boost { namespace python { namespace detail { 

struct signature_element
{
    char const* basename;
    converter::pytype_function pytype_f;
    bool lvalue;
};

struct py_func_sig_info
{
    signature_element const *signature;
    signature_element const *ret;
};


template<class Sig> struct signature;

template<class... Args>
struct signature<type_list<Args...>>
{
    static signature_element const* elements()
    {
        static signature_element const result[] = {
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
            { type_id<Args>().name(),
              &converter::expected_pytype_for_arg<Args>::get_pytype,
              indirect_traits::is_reference_to_non_const<Args>::value }...,
#else
            { type_id<Args>().name(),
              0, 
              indirect_traits::is_reference_to_non_const<Args>::value }...,
#endif
            {0, 0, 0}
        };
        
        return result;
    }
};

}}} // namespace boost::python::detail

#endif // SIGNATURE_DWA20021121_HPP
