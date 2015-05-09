// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef DICT_20020706_HPP
#define DICT_20020706_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/object.hpp>
# include <boost/python/list.hpp>
# include <boost/python/tuple.hpp>
# include <boost/python/extract.hpp>
# include <boost/python/converter/pytype_object_mgr_traits.hpp>

namespace boost { namespace python {

class dict : public object {
public:
    // dict() -> new empty dictionary.
    // dict(mapping) -> new dictionary initialized from a mapping object's (key, value) pairs.
    // dict(seq) -> new dictionary initialized as if via:
    dict() : object{detail::new_reference(PyDict_New())} {}

    template<class T>
    explicit dict(T&& data) : dict{call(object{std::forward<T>(data)})} {}

public:
    // D.clear() -> None. Remove all items.
    void clear() {
        if (is_dict_exact())
            PyDict_Clear(ptr());
        else
            attr("clear")();
    }

    // D.copy() -> dict. A shallow copy.
    dict copy() const {
        if (is_dict_exact())
            return dict{detail::new_reference(PyDict_Copy(ptr()))};
        else
            return dict{detail::borrowed_reference(attr("copy")().ptr())};
    }

    // D.get(k[,d]) -> D[k] if D.contains(k), else d. d defaults to None.
    template<class T, class... O>
    object get(T&& key, O&&... default_) const {
        static_assert(sizeof...(O) <= 1, "");
        if (is_dict_exact()) {
            PyObject* result = PyDict_GetItem(ptr(), object{std::forward<T>(key)}.ptr());
            return result ? object{detail::borrowed_reference(result)}
                          : object{std::forward<O>(default_)...};
        }
        else {
            return attr("get")(std::forward<T>(key), std::forward<O>(default_)...);
        }
    }

    // D.has_key(k) -> bool. Deprecated in favor of D.contains(k).
    template<class T>
    [[deprecated]] bool has_key(T&& key) const {
        return extract<bool>{contains(std::forward<T>(key))};
    };

    // D.items() -> list of (key, value) pairs, as 2-tuples.
    list items() const {
        if (is_dict_exact())
            return list{detail::new_reference(PyDict_Items(ptr()))};
        else
            return list{detail::borrowed_reference(attr("items")().ptr())};
    }

    // D.keys() -> list of keys.
    list keys() const {
        if (is_dict_exact())
            return list{detail::new_reference(PyDict_Keys(ptr()))};
        else
            return list{detail::borrowed_reference(attr("keys")().ptr())};
    }

    // D.pop(k[,d]) -> D[k] or d. Remove and return D[k] if D.contains(k).
    // Return d otherwise; Raise KeyError if d is not given and !D.contains(k).
    template<class T, class... O>
    object pop(T&& key, O&&... default_) const {
        static_assert(sizeof...(O) <= 1, "");
        return attr("pop")(std::forward<T>(key), std::forward<O>(default_)...);
    }

    // D.popitem() -> (k, v). Remove and return some (key, value) pair as a
    // 2-tuple; but raise KeyError if D is empty.
    tuple popitem() {
        return tuple{detail::borrowed_reference(attr("popitem")().ptr())};
    };

    // D.setdefault(k[,d]) -> D.get(k[,d]). Also set D[k]=d if !D.contains(k).
    template<class T, class... O>
    object setdefault(T&& key, O&&... default_) const {
        static_assert(sizeof...(O) <= 1, "");
        return attr("setdefault")(std::forward<T>(key), std::forward<O>(default_)...);
    }

    // D.update(E) -> None. Update D from E: for k in E.keys(): D[k] = E[k].
    template<class T>
    void update(T&& other) {
        if (is_dict_exact()) {
            if (PyDict_Update(ptr(), object{std::forward<T>(other)}.ptr()) == -1)
                throw_error_already_set();
        }
        else {
            attr("update")(std::forward<T>(other));
        }
    }

    // D.values() -> list of values.
    list values() const {
        if (is_dict_exact())
            return list{detail::new_reference(PyDict_Values(ptr()))};
        else
            return list{detail::borrowed_reference(attr("values")().ptr())};
    }

#if PY_MAJOR_VERSION < 3
    // Python 2 iterator versions of items, keys and values
    object iteritems() const { return attr("iteritems")(); }
    object iterkeys() const { return attr("iterkeys")(); }
    object itervalues() const { return attr("itervalues")(); }
#endif

public: // implementation detail -- for internal use only
    BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(dict, object)

private:
    static detail::new_reference call(object const& arg) {
        return (detail::new_reference)PyObject_CallFunctionObjArgs(
            (PyObject*)&PyDict_Type, arg.ptr(), nullptr
        );
    }

    bool is_dict_exact() const noexcept { return PyDict_CheckExact(ptr()); }
};

//
// Converter Specializations
//
namespace converter {
    template<>
    struct object_manager_traits<dict>
        : pytype_object_manager_traits<&PyDict_Type, dict>
    {};
}

}}   // namespace boost::python

#endif
