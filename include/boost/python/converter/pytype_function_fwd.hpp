#pragma once
#include <boost/python/detail/prefix.hpp>

namespace boost { namespace python { namespace converter {

using pytype_function = PyTypeObject const* (*)();

}}} // namespace boost::python::converter
