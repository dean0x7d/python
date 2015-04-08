// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef RETURN_FROM_PYTHON_DWA200265_HPP
# define RETURN_FROM_PYTHON_DWA200265_HPP

# include <boost/python/converter/from_python.hpp>
# include <boost/python/converter/registered.hpp>
# include <boost/python/converter/object_manager.hpp>
# include <boost/python/detail/referent_storage.hpp>
# include <boost/python/errors.hpp>
# include <boost/python/handle.hpp>

namespace boost { namespace python { namespace converter { 

namespace detail {
    template<class T>
    struct return_rvalue_from_python {
        using result_type = T;

        return_rvalue_from_python()
            : m_data(const_cast<registration*>(&registered<T>::converters))
        {}

        result_type operator()(PyObject* p) {
            // Take possession of the source object here. If the result is in
            // fact going to be a copy of an lvalue embedded in the object,
            // and we take possession inside rvalue_result_from_python, it
            // will be destroyed too early.
            handle<> holder(p);

            return *static_cast<result_type*>(rvalue_result_from_python(p, m_data.stage1));
        }

    private:
        rvalue_from_python_data<T> m_data;
    };

    template<class T>
    struct return_object_manager_from_python {
        using result_type = T;

        result_type operator()(PyObject* p) const {
            return static_cast<result_type>(
                object_manager_traits<T>::adopt(expect_non_null(p))
            );
        }
    };
} // namespace detail

template<class T>
struct return_from_python : cpp14::conditional_t<
    is_object_manager<T>::value,
    detail::return_object_manager_from_python<T>,
    detail::return_rvalue_from_python<T>
> {};

template<class T>
struct return_from_python<T*> {
    using result_type = T*;

    result_type operator()(PyObject* p) const {
        return static_cast<result_type>(
            pointer_result_from_python(p, registered<T>::converters)
        );
    }
};

template<class T>
struct return_from_python<T&> {
    using result_type = T&;

    result_type operator()(PyObject* p) const {
        return python::detail::void_ptr_to_reference<result_type>(
            reference_result_from_python(p, registered<T>::converters)
        );
    }
};

// Specialization as a convenience for call and call_method
template<>
struct return_from_python<void> {
    using result_type = void;

    result_type operator()(PyObject* p) const {
        void_result_from_python(p);
    }
};

}}} // namespace boost::python::converter

#endif // RETURN_FROM_PYTHON_DWA200265_HPP
