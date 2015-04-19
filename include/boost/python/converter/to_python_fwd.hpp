#pragma once

namespace boost { namespace python {

namespace converter {
    template<class T>
    struct arg_to_python;
}

template<class T>
struct to_python_value;

}} // namespace boost::python
