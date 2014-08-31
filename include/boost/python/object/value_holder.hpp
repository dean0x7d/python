// Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

# ifndef VALUE_HOLDER_DWA20011215_HPP
#  define VALUE_HOLDER_DWA20011215_HPP 

#  include <boost/python/instance_holder.hpp>
#  include <boost/python/type_id.hpp>
#  include <boost/python/wrapper.hpp>

#  include <boost/python/object/inheritance_query.hpp>
#  include <boost/python/object/forward.hpp>

#  include <boost/python/detail/force_instantiate.hpp>

#  include <boost/utility/addressof.hpp>

namespace boost { namespace python { namespace objects { 

template <class Value, class Held = Value, bool has_back_reference = false>
struct value_holder : instance_holder
{
    typedef Value held_type;
    typedef Value value_type;

    // Forward construction to the held object
    template <class... As>
    value_holder(PyObject* self, As&&... as)
    : m_held(objects::do_unforward(std::forward<As>(as),0)...)
    {
        python::detail::initialize_wrapper(self, boost::addressof(this->m_held));
    }


 private: // required holder implementation
    void* holds(type_info, bool null_ptr_only);
    
    template <class T>
    inline void* holds_wrapped(type_info dst_t, wrapper<T>*,T* p)
    {
        return python::type_id<T>() == dst_t ? p : 0;
    }
    
    inline void* holds_wrapped(type_info, ...)
    {
        return 0;
    }
 private: // data members
    Value m_held;
};

template <class Value, class Held>
struct value_holder<Value, Held, true> : instance_holder
{
    typedef Held held_type;
    typedef Value value_type;
    
    // Forward construction to the held object
    template <class... As>
    value_holder(PyObject* p, As&&... as)
    : m_held(p, objects::do_unforward(std::forward<As>(as),0)...) {}

private: // required holder implementation
    void* holds(type_info, bool null_ptr_only);

 private: // data members
    Held m_held;
};


template <class Value, class Held, bool has_back_reference>
void* value_holder<Value, Held, has_back_reference>::holds(type_info dst_t, bool /*null_ptr_only*/)
{
    if (void* wrapped = holds_wrapped(dst_t, boost::addressof(m_held), boost::addressof(m_held)))
        return wrapped;
    
    type_info src_t = python::type_id<Value>();
    return src_t == dst_t ? boost::addressof(m_held)
        : find_static_type(boost::addressof(m_held), src_t, dst_t);
}

template <class Value, class Held>
void* value_holder<Value,Held,true>::holds(type_info dst_t, bool /*null_ptr_only*/)
{
    type_info src_t = python::type_id<Value>();
    Value* x = &m_held;
    
    if (dst_t == src_t)
        return x;
    else if (dst_t == python::type_id<Held>())
        return &m_held;
    else
        return find_static_type(x, src_t, dst_t);
}

}}} // namespace boost::python::objects

# endif // VALUE_HOLDER_DWA20011215_HPP
