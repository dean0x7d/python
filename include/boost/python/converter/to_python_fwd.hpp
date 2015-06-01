#pragma once

namespace boost { namespace python {

namespace converter {
    template<class T>
    struct arg_to_python;
}

template<class T>
struct to_python_value;

template<class T>
struct to_python {
    static constexpr PyTypeObject const* pytype = nullptr;
};

}} // namespace boost::python
