// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef EXTRACT_DWA200265_HPP
# define EXTRACT_DWA200265_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/converter/object_manager.hpp>
# include <boost/python/converter/from_python.hpp>
# include <boost/python/converter/rvalue_from_python_data.hpp>
# include <boost/python/converter/registered.hpp>

# include <boost/python/object_core.hpp>
# include <boost/python/refcount.hpp>

# include <boost/python/detail/copy_ctor_mutates_rhs.hpp>
# include <boost/python/detail/referent_storage.hpp>

namespace boost { namespace python {

namespace converter {
    template<class T>
    struct extract_rvalue {
        using result_type = cpp14::conditional_t<
            python::detail::copy_ctor_mutates_rhs<T>::value,
            T&,
            T const&
        >;

        extract_rvalue(const extract_rvalue&) = delete;
        extract_rvalue& operator=(const extract_rvalue&) = delete;

        extract_rvalue(PyObject* p)
            : m_source(p),
              m_data(rvalue_from_python_stage1(p, registered<T>::converters))
        {}

        bool check() const { return m_data.stage1.convertible != nullptr; }

        result_type operator()() const {
            return *static_cast<T*>(
                // Only do the stage2 conversion once
                (m_data.stage1.convertible == m_data.storage.bytes)
                ? m_data.storage.bytes
                : rvalue_from_python_stage2(m_source, m_data.stage1, registered<T>::converters)
            );
        }

    private:
        PyObject* m_source;
        mutable rvalue_from_python_data<T> m_data;
    };

    template<class T>
    struct extract_object_manager {
        using result_type = T;

        extract_object_manager(PyObject* p) : m_source(p) {}

        bool check() const { return object_manager_traits<result_type>::check(m_source); }

        result_type operator()() const {
            return static_cast<result_type>(
                object_manager_traits<result_type>::adopt(incref(m_source))
            );
        }

    private:
        PyObject* m_source;
    };

    template<class T>
    using select_extract_t = cpp14::conditional_t<
        is_object_manager<T>::value,
        extract_object_manager<T>,
        extract_rvalue<T>
    >;

    template<class T>
    struct extract_base : select_extract_t<T> {
        using select_extract_t<T>::select_extract_t;
    };

    template<class T>
    struct extract_base<T*> {
        using result_type = T*;

        extract_base(PyObject* p)
            : m_source(p),
              m_result(
                  (p == Py_None)
                  ? nullptr
                  : get_lvalue_from_python(p, registered<T>::converters)
              )
        {}

        bool check() const { return m_source == Py_None || m_result != nullptr; }

        result_type operator()() const {
            if (m_result == nullptr && m_source != Py_None)
                throw_no_pointer_from_python(m_source, registered<T>::converters);

            return static_cast<result_type>(m_result);
        }

    private:
        PyObject* m_source;
        void* m_result;
    };

    template<class T>
    struct extract_base<T&> {
        using result_type = T&;

        extract_base(PyObject* p)
            : m_source(p),
              m_result(get_lvalue_from_python(p, registered<T>::converters))
        {}

        bool check() const { return m_result != nullptr; }

        result_type operator()() const {
            if (m_result == nullptr)
                throw_no_reference_from_python(m_source, registered<T>::converters);

            return python::detail::void_ptr_to_reference<result_type>(m_result);
        }

    private:
        PyObject* m_source;
        void* m_result;
    };
} // namespace converter

template<class T>
struct extract : converter::extract_base<T> {
private:
    using base = converter::extract_base<T>;

public:
    using result_type = typename base::result_type;

    operator result_type() const { return (*this)(); }
    
    extract(PyObject* p) : base(p) {}
    extract(object const& x) : base(x.ptr()) {}
};

}} // namespace boost::python::converter

#endif // EXTRACT_DWA200265_HPP
