#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/tuple.hpp>

#include <tuple>
#include <string>

using namespace boost::python;

std::tuple<int, float, double> tuple_return_to_python() {
	return std::make_tuple(1, 2.0f, 3.0);
}

void tuple_arg_to_python(object func) {
	func(std::make_tuple("char const*", std::string("std::string")));
}

tuple tuple_return_from_python(tuple pt) {
	auto t = extract<std::tuple<int, int>>{pt}();
	return make_tuple(std::get<0>(t), std::get<1>(t));
}

tuple tuple_arg_from_python(std::tuple<int, int> t) {
	return make_tuple(std::get<0>(t), std::get<1>(t));
}

int test_empty(std::tuple<>) { return 1; }

BOOST_PYTHON_MODULE(std_tuple_ext) {
    def("tuple_return_to_python", tuple_return_to_python);
    def("tuple_arg_to_python", tuple_arg_to_python);
    def("tuple_return_from_python", tuple_return_from_python);
    def("tuple_arg_from_python", tuple_arg_from_python);
    def("test_empty", test_empty);
}
