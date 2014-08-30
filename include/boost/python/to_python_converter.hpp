// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TO_PYTHON_CONVERTER_DWA200221_HPP
# define TO_PYTHON_CONVERTER_DWA200221_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/converter/registry.hpp>
# include <boost/python/converter/as_to_python_function.hpp>
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
# include <boost/python/converter/pytype_function.hpp>
#endif
# include <boost/python/type_id.hpp>

namespace boost { namespace python { 

template < class T, class Conversion, bool has_get_pytype=false >
struct to_python_converter 
{
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    using HasGetPytype = std::integral_constant<bool, has_get_pytype>;

    static PyTypeObject const* get_pytype_1(std::true_type)
    {
        return Conversion::get_pytype();
    }

    static PyTypeObject const* get_pytype_1(std::false_type)
    {
        return nullptr;
    }
    
    static PyTypeObject const* get_pytype_impl()
    {
        return get_pytype_1(HasGetPytype());
    }
#endif
    
    to_python_converter();
};

//
// implementation
//

template <class T, class Conversion ,bool has_get_pytype>
to_python_converter<T,Conversion, has_get_pytype>::to_python_converter()
{
    using normalized = converter::as_to_python_function<T, Conversion>;

    converter::registry::insert(
        &normalized::convert
        , type_id<T>()
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        , &get_pytype_impl
#endif
        );
}

}} // namespace boost::python

#endif // TO_PYTHON_CONVERTER_DWA200221_HPP

