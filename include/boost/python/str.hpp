// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef STR_20020703_HPP
#define STR_20020703_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/object.hpp>
# include <boost/python/list.hpp>
# include <boost/python/converter/pytype_object_mgr_traits.hpp>

// disable defines in <cctype> provided by some system libraries
#undef isspace
#undef islower
#undef isalpha
#undef isdigit
#undef isalnum
#undef isupper

namespace boost { namespace python {

class str : public object
{
public:
    str(const char* s = "") : object(detail::new_reference(_PyString_FromString(s))) {}

    str(char const* start, char const* finish)
        : object(detail::new_reference(
            _PyString_FromStringAndSize(start, str_size_as_py_ssize_t(finish - start))
        ))
    {}

    str(char const* start, std::size_t length)
        : object(detail::new_reference(
            _PyString_FromStringAndSize(start, str_size_as_py_ssize_t(length))
        ))
    {}

    template <class T>
    explicit str(T&& other) : str{call(object{std::forward<T>(other)})} {}

public:
    str capitalize() const { return str_call("capitalize"); }

    template <class T>
    str center(T&& width) const {
        return str_call("center", std::forward<T>(width));
    }

    template<class T>
    long count(T&& sub) const {
        return int_call("count", std::forward<T>(sub));
    }
    template<class T1, class T2>
    long count(T1&& sub, T2&& start) const {
        return int_call("count", std::forward<T1>(sub), std::forward<T2>(start));
    }
    template<class T1, class T2, class T3>
    long count(T1&& sub, T2&& start, T3&& end) const {
        return int_call("count", std::forward<T1>(sub), std::forward<T2>(start), std::forward<T3>(end));
    }

#if PY_VERSION_HEX < 0x03000000
    object decode() const {
        return this->attr("decode")();
    }
    template <class T>
    object decode(T&& encoding) const {
        return this->attr("decode")(std::forward<T>(encoding));
    }
    template <class T1, class T2>
    object decode(T1&& encoding, T2&& errors) const {
        return this->attr("decode")(std::forward<T1>(encoding), std::forward<T2>(errors));
    }
#endif

    object encode() const {
        return this->attr("encode")();
    }
    template <class T>
    object encode(T&& encoding) const {
        return this->attr("encode")(std::forward<T>(encoding));
    }
    template <class T1, class T2>
    object encode(T1&& encoding, T2&& errors) const {
        return this->attr("encode")(std::forward<T1>(encoding), std::forward<T2>(errors));
    }

    template <class T>
    bool endswith(T&& suffix) const {
        return int_call("endswith", std::forward<T>(suffix));
    }
    template <class T1, class T2>
    bool endswith(T1&& suffix, T2&& start) const {
        return int_call("endswith", std::forward<T1>(suffix), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    bool endswith(T1&& suffix, T2&& start, T3&& end) const {
        return int_call("endswith", std::forward<T1>(suffix), std::forward<T2>(start), std::forward<T3>(end));
    }
    
    str expandtabs() const {
        return str_call("expandtabs");
    }
    template <class T>
    str expandtabs(T&& tabsize) const {
        return str_call("expandtabs", std::forward<T>(tabsize));
    }
    
    template <class T>
    long find(T&& sub) const {
        return int_call("find", std::forward<T>(sub));
    }
    template <class T1, class T2>
    long find(T1&& sub, T2&& start) const {
        return int_call("find", std::forward<T1>(sub), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    long find(T1&& sub, T2&& start, T3&& end) const {
        return int_call("find", std::forward<T1>(sub), std::forward<T2>(start), std::forward<T3>(end));
    }
    
    template <class T>
    long index(T&& sub) const {
        return int_call("index", std::forward<T>(sub));
    }
    template <class T1, class T2>
    long index(T1&& sub, T2&& start) const {
        return int_call("index", std::forward<T1>(sub), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    long index(T1&& sub, T2&& start, T3&& end) const {
        return int_call("index", std::forward<T1>(sub), std::forward<T2>(start), std::forward<T3>(end));
    }

    bool isalnum() const { return int_call("isalnum"); }
    bool isalpha() const { return int_call("isalpha"); }
    bool isdigit() const { return int_call("isdigit"); }
    bool islower() const { return int_call("islower"); }
    bool isspace() const { return int_call("isspace"); }
    bool istitle() const { return int_call("istitle"); }
    bool isupper() const { return int_call("isupper"); }

    template <class T>
    str join(T&& sequence) const {
        return str_call("join", std::forward<T>(sequence));
    }
    
    template <class T>
    str ljust(T&& width) const {
        return str_call("ljust", std::forward<T>(width));
    }

    str lower() const { return str_call("lower"); }
    str lstrip() const { return str_call("lstrip"); }

    template <class T1, class T2>
    str replace(T1&& old, T2&& new_) const {
        return str_call("replace", std::forward<T1>(old), std::forward<T2>(new_));
    }
    template <class T1, class T2, class T3>
    str replace(T1&& old, T2&& new_, T3&& maxsplit) const {
        return str_call("replace", std::forward<T1>(old), std::forward<T2>(new_), std::forward<T3>(maxsplit));
    }
    
    template <class T>
    long rfind(T&& sub) const {
        return int_call("rfind", std::forward<T>(sub));
    }
    template <class T1, class T2>
    long rfind(T1&& sub, T2&& start) const {
        return int_call("rfind", std::forward<T1>(sub), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    long rfind(T1&& sub, T2&& start, T3&& end) const {
        return int_call("rfind", std::forward<T1>(sub), std::forward<T2>(start), std::forward<T3>(end));
    }
    
    template <class T>
    long rindex(T&& sub) const {
        return int_call("rindex", std::forward<T>(sub));
    }
    template <class T1, class T2>
    long rindex(T1&& sub, T2&& start) const {
        return int_call("rindex", std::forward<T1>(sub), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    long rindex(T1&& sub, T2&& start, T3&& end) const {
        return int_call("rindex", std::forward<T1>(sub), std::forward<T2>(start), std::forward<T3>(end));
    }

    template <class T>
    str rjust(T&& width) const {
        return str_call("rjust", std::forward<T>(width));
    }

    str rstrip() const { return str_call("rstrip"); }

    list split() const {
        return list(this->attr("split")());
    }
    template <class T>
    list split(T&& sep) const {
        return list(this->attr("split")(std::forward<T>(sep)));
    }
    template <class T1, class T2>
    list split(T1&& sep, T2&& maxsplit) const {
        return list(this->attr("split")(std::forward<T1>(sep), std::forward<T2>(maxsplit)));
    }

    list splitlines() const {
        return list(this->attr("splitlines")());
    }
    template <class T>
    list splitlines(T&& keepends) const {
        return list(this->attr("splitlines")(std::forward<T>(keepends)));
    }

    template <class T>
    bool startswith(T&& prefix) const {
        return int_call("startswith", std::forward<T>(prefix));
    }
    template <class T1, class T2>
    bool startswith(T1&& prefix, T2&& start) const {
        return int_call("startswith", std::forward<T1>(prefix), std::forward<T2>(start));
    }
    template <class T1, class T2, class T3>
    bool startswith(T1&& prefix, T2&& start, T3&& end) const {
        return int_call("startswith", std::forward<T1>(prefix), std::forward<T2>(start), std::forward<T3>(end));
    }

    str strip() const { return str_call("strip"); }
    str swapcase() const { return str_call("swapcase"); }
    str title() const { return str_call("title"); }

    template <class T>
    str translate(T&& table) const {
        return str_call("translate", std::forward<T>(table));
    }
    template <class T1, class T2>
    str translate(T1&& table, T2&& deletechars) const {
        return str_call("translate", std::forward<T1>(table), std::forward<T2>(deletechars));
    }

public: // implementation detail -- for internal use only
    BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(str, object)

private:
    static detail::new_reference call(object const& arg) {
        return detail::new_reference(
            PyObject_CallFunctionObjArgs((PyObject*)&_PyString_Type, arg.ptr(), nullptr)
        );
    }

    template<class... Args>
    str str_call(char const* name, Args&&... args) const {
        handle<> method_name(_PyString_FromString(name));
        return str(detail::new_reference(expect_non_null(
            PyObject_CallMethodObjArgs(
                this->ptr(),
                method_name.get(),
                object(std::forward<Args>(args)).ptr()...,
                nullptr
            )
        )));
    }

    template<class... Args>
    long int_call(char const* name, Args&&... args) const {
#if PY_VERSION_HEX >= 0x03000000
        long result = PyLong_AsLong(this->attr(name)(std::forward<Args>(args)...).ptr());
#else
        long result = PyInt_AsLong(this->attr(name)(args...).ptr());
#endif
        if (PyErr_Occurred())
            throw_error_already_set();
        return result;
    }

    static ssize_t str_size_as_py_ssize_t(std::size_t n) {
        if (n > static_cast<std::size_t>(ssize_t_max)) {
            throw std::range_error("str size > ssize_t_max");
        }
        return static_cast<ssize_t>(n);
    }
};

//
// Converter Specializations
//
namespace converter {
  template <>
  struct object_manager_traits<str>
      : pytype_object_manager_traits<&_PyString_Type, str>
  {};
}

}}  // namespace boost::python

#endif // STR_20020703_HPP
