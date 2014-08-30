// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef TRANSLATE_EXCEPTION_TDS20091020_HPP
# define TRANSLATE_EXCEPTION_TDS20091020_HPP

# include <boost/python/detail/exception_handler.hpp>

# include <boost/call_traits.hpp>
# include <boost/python/cpp14/type_traits.hpp>

# include <boost/function/function0.hpp>

namespace boost { namespace python { namespace detail { 

// A ternary function object used to translate C++ exceptions of type
// ExceptionType into Python exceptions by invoking an object of type
// Translate. Typically the translate function will be curried with
// boost::bind().
template <class ExceptionType, class Translate>
struct translate_exception
{
    using exception_cref = 
        cpp14::add_lvalue_reference_t<
            cpp14::add_const_t<
                ExceptionType
            >
        >;
    
    inline bool operator()(
        exception_handler const& handler
      , function0<void> const& f
      , typename call_traits<Translate>::param_type translate) const
    {
        try
        {
            return handler(f);
        }
        catch(exception_cref e)
        {
            translate(e);
            return true;
        }
    }
};

}}} // namespace boost::python::detail

#endif // TRANSLATE_EXCEPTION_DWA2002810_HPP
