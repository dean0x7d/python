// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAKE_HOLDER_DWA20011215_HPP
# define MAKE_HOLDER_DWA20011215_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/object/instance.hpp>
#if !defined( BOOST_PYTHON_NO_PY_SIGNATURES) && defined( BOOST_PYTHON_PY_SIGNATURES_PROPER_INIT_SELF_TYPE)
# include <boost/python/detail/python_type.hpp>
#endif

namespace boost { namespace python { namespace objects {

template<class Holder, class Sig>
struct make_holder;

template<class Holder, class... Args>
struct make_holder<Holder, detail::type_list<Args...>>
{
    using object_t = 
#if !defined( BOOST_PYTHON_NO_PY_SIGNATURES) && defined( BOOST_PYTHON_PY_SIGNATURES_PROPER_INIT_SELF_TYPE)
        boost::python::detail::python_class<typename Holder::value_type>;
#else
        PyObject;
#endif

    static void execute(object_t* p, Args const&... args)
    {
        using instance_t = instance<Holder>;
        
        void* memory = Holder::allocate(p, offsetof(instance_t, storage), sizeof(Holder));
        try {
            // TODO: perfect forwarding
            (new (memory) Holder(p, args...))->install(p);
        }
        catch(...) {
            Holder::deallocate(p, memory);
            throw;
        }
    }
};


}}} // namespace boost::python::objects

# endif // MAKE_HOLDER_DWA20011215_HPP
