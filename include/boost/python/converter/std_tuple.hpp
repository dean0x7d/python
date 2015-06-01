#pragma once
#include <boost/python/converter/to_python_fwd.hpp>
#include <boost/python/converter/arg_from_python.hpp>

#include <boost/python/cpp14/utility.hpp>

#include <tuple>

namespace boost { namespace python { namespace converter {

template<class Tuple, class = cpp14::make_index_sequence<std::tuple_size<Tuple>::value>>
struct std_tuple_from_python;

template<class... Ts, std::size_t... Is>
struct std_tuple_from_python<std::tuple<Ts...>, cpp14::index_sequence<Is...>> {
    using tuple_t = std::tuple<Ts...>;

    std_tuple_from_python() {
        registry::insert_rvalue_converter(
            &convertible, &construct, type_id<tuple_t>(), &PyTuple_Type
        );
    }

    static void* convertible(PyObject* source) {
        if (!PyTuple_Check(source))
            return nullptr;
        if (PyTuple_GET_SIZE(source) != sizeof...(Ts))
            return nullptr;

        for (bool check : {arg_from_python<Ts>{PyTuple_GET_ITEM(source, Is)}.check()..., true}) {
            if (!check)
                return nullptr;
        }

        return source;
    }

    static void construct(PyObject* source, rvalue_from_python_stage1_data* data) {
        void* storage = ((rvalue_from_python_storage<tuple_t>*)data)->storage.bytes;

        new (storage) tuple_t(arg_from_python<Ts>{PyTuple_GET_ITEM(source, Is)}()...);

        data->convertible = storage;
    }
};

template<class... Ts>
struct rvalue_from_python_register<std::tuple<Ts...>> {
    static std_tuple_from_python<std::tuple<Ts...>> register_;

    rvalue_from_python_register() {
        [](...){}(register_); // trigger static initialization of std_tuple_from_python
    }
};

template<class... Ts>
std_tuple_from_python<std::tuple<Ts...>> rvalue_from_python_register<std::tuple<Ts...>>::register_;

} // namespace converter

template<class... Ts>
struct to_python_value<std::tuple<Ts...>> {
    PyObject* operator()(std::tuple<Ts...> const& source) const {
        return convert_impl(source, cpp14::make_index_sequence<sizeof...(Ts)>{});
    }

    template<std::size_t... Is>
    static PyObject* convert_impl(std::tuple<Ts...> const& source, cpp14::index_sequence<Is...>) {
        return PyTuple_Pack(
            sizeof...(Ts), converter::arg_to_python<Ts>{std::get<Is>(source)}.get()...
        );
    }
};

template<class... Ts>
struct to_python<std::tuple<Ts...>> {
    static constexpr PyTypeObject const* pytype = &PyTuple_Type;
};

}} // namespace boost::python
