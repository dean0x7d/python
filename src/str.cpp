// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/str.hpp>

namespace boost { namespace python {

static struct register_str_pytype_ptr
{
    register_str_pytype_ptr()
    {
        const_cast<converter::registration &>(
            converter::registry::lookup(boost::python::type_id<boost::python::str>())
            )
#if PY_VERSION_HEX >= 0x03000000
            .m_class_object = &PyUnicode_Type;
#else
            .m_class_object = &PyString_Type;
#endif
    }
}register_str_pytype_ptr_;
    
}}  // namespace boost::python
