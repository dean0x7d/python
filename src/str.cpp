// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/str.hpp>
#include <boost/python/extract.hpp>

namespace boost { namespace python { namespace detail {

detail::new_reference str_base::call(object const& arg_)
{
    return (detail::new_reference)PyObject_CallFunction(
#if PY_VERSION_HEX >= 0x03000000
        (PyObject*)&PyUnicode_Type,
#else
        (PyObject*)&PyString_Type, 
#endif
        const_cast<char*>("(O)"), 
        arg_.ptr());
}

template<class... Args>
str str_base::str_call(char const* name, Args... args) const
{
#if PY_VERSION_HEX >= 0x03000000
    handle<> method_name(PyUnicode_FromString(name));
#else
          handle<> method_name(PyString_FromString(name));
#endif
    return str(new_reference(expect_non_null(
        PyObject_CallMethodObjArgs(this->ptr(), method_name.get(), args.ptr()..., nullptr)
    )));
}

str_base::str_base()
  : object(detail::new_reference(
#if PY_VERSION_HEX >= 0x03000000
              ::PyUnicode_FromString("")
#else
              ::PyString_FromString("")
#endif
            ))
{}

str_base::str_base(const char* s)
  : object(detail::new_reference(
#if PY_VERSION_HEX >= 0x03000000
              ::PyUnicode_FromString(s)
#else
              ::PyString_FromString(s)
#endif
            ))
{}

namespace {

    ssize_t str_size_as_py_ssize_t(std::size_t n)
    {
      if (n > static_cast<std::size_t>(ssize_t_max))
      {
          throw std::range_error("str size > ssize_t_max");
      }
      return static_cast<ssize_t>(n);
    }

} // namespace <anonymous>

str_base::str_base(char const* start, char const* finish)
    : object(
        detail::new_reference(
#if PY_VERSION_HEX >= 0x03000000
            ::PyUnicode_FromStringAndSize
#else
            ::PyString_FromStringAndSize
#endif
                (start, str_size_as_py_ssize_t(finish - start))
        )
    )
{}

str_base::str_base(char const* start, std::size_t length) // new str
    : object(
        detail::new_reference(
#if PY_VERSION_HEX >= 0x03000000
            ::PyUnicode_FromStringAndSize
#else
            ::PyString_FromStringAndSize
#endif
            ( start, str_size_as_py_ssize_t(length) )
        )
    )
{}

str_base::str_base(object_cref other)
    : object(str_base::call(other))
{}

str str_base::capitalize() const
{
    return str_call("capitalize");
}

str str_base::center(object_cref width) const
{
    return str_call("center", width);
}

long str_base::count(object_cref sub) const
{
    return extract<long>(this->attr("count")(sub));
}

long str_base::count(object_cref sub, object_cref start) const
{
    return extract<long>(this->attr("count")(sub,start));
}

long str_base::count(object_cref sub, object_cref start, object_cref end) const
{
    return extract<long>(this->attr("count")(sub,start,end));
}

#if PY_VERSION_HEX < 0x03000000
object str_base::decode() const
{
    return this->attr("decode")();
}

object str_base::decode(object_cref encoding) const
{
    return this->attr("decode")(encoding);
}

object str_base::decode(object_cref encoding, object_cref errors) const
{
    return this->attr("decode")(encoding,errors);
}
#endif

object str_base::encode() const
{
    return this->attr("encode")();
}

object str_base::encode(object_cref encoding) const
{
    return this->attr("encode")(encoding);
}

object str_base::encode(object_cref encoding, object_cref errors) const
{
    return this->attr("encode")(encoding,errors);
}


#if PY_VERSION_HEX >= 0x03000000
    #define _BOOST_PYTHON_ASLONG PyLong_AsLong
#else
    #define _BOOST_PYTHON_ASLONG PyInt_AsLong
#endif

bool str_base::endswith(object_cref suffix) const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("endswith")(suffix).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

str str_base::expandtabs() const
{
    return str_call("expandtabs");
}

str str_base::expandtabs(object_cref tabsize) const
{
    return str_call("expandtabs", tabsize);
}

long str_base::find(object_cref sub) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("find")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::find(object_cref sub, object_cref start) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("find")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::find(object_cref sub, object_cref start, object_cref end) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("find")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("index")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub, object_cref start) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("index")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::index(object_cref sub, object_cref start, object_cref end) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("index")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isalnum() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("isalnum")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isalpha() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("isalpha")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isdigit() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("isdigit")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::islower() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("islower")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isspace() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("isspace")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::istitle() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("istitle")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::isupper() const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("isupper")().ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

str str_base::join(object_cref sequence) const
{
    return str_call("join", sequence);
}

str str_base::ljust(object_cref width) const
{
    return str_call("ljust", width);
}

str str_base::lower() const
{
    return str_call("lower");
}

str str_base::lstrip() const
{
    return str_call("lstrip");
}

str str_base::replace(object_cref old, object_cref new_) const
{
    return str_call("replace", old, new_);
}

str str_base::replace(object_cref old, object_cref new_, object_cref maxsplit) const
{
    return str_call("replace", old, new_, maxsplit);
}

long str_base::rfind(object_cref sub) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rfind")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rfind(object_cref sub, object_cref start) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rfind")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rfind(object_cref sub, object_cref start, object_cref end) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rfind")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rindex")(sub).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub, object_cref start) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rindex")(sub,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

long str_base::rindex(object_cref sub, object_cref start, object_cref end) const
{
    long result = _BOOST_PYTHON_ASLONG(this->attr("rindex")(sub,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

str str_base::rjust(object_cref width) const
{
    return str_call("rjust", width);
}

str str_base::rstrip() const
{
    return str_call("rstrip");
}

list str_base::split() const
{
    return list(this->attr("split")());
}

list str_base::split(object_cref sep) const
{
    return list(this->attr("split")(sep));
}

list str_base::split(object_cref sep, object_cref maxsplit) const
{
    return list(this->attr("split")(sep,maxsplit));
}

list str_base::splitlines() const
{
    return list(this->attr("splitlines")());
}

list str_base::splitlines(object_cref keepends) const
{
    return list(this->attr("splitlines")(keepends));
}

bool str_base::startswith(object_cref prefix) const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("startswith")(prefix).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::startswith(object_cref prefix, object_cref start) const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("startswith")(prefix,start).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

bool str_base::startswith(object_cref prefix, object_cref start, object_cref end) const
{
    bool result = _BOOST_PYTHON_ASLONG(this->attr("startswith")(prefix,start,end).ptr());
    if (PyErr_Occurred())
        throw_error_already_set();
    return result;
}

#undef _BOOST_PYTHON_ASLONG

str str_base::strip() const
{
    return str_call("strip");
}

str str_base::swapcase() const
{
    return str_call("swapcase");
}

str str_base::title() const
{
    return str_call("title");
}

str str_base::translate(object_cref table) const
{
    return str_call("translate", table);
}

str str_base::translate(object_cref table, object_cref deletechars) const
{
    return str_call("translate", table, deletechars);
}

str str_base::upper() const
{
    return str_call("upper");
}

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
    
}}}  // namespace boost::python
