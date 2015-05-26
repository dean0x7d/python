//  Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/converter/registry.hpp>
#include <boost/python/converter/builtin_converters.hpp>

#include <set>

#if defined(BOOST_PYTHON_TRACE_REGISTRY)
# include <iostream>
#endif

namespace boost { namespace python { namespace converter {

BOOST_PYTHON_DECL PyTypeObject const* registration::expected_from_python_type() const
{
    if (m_class_object)
        return m_class_object;

    std::set<PyTypeObject const*> pool;
    for (auto const& rvalue_converter : rvalue_chain) {
        if (rvalue_converter.expected_pytype)
            pool.insert(rvalue_converter.expected_pytype());
    }

    //for now I skip the search for common base
    if (pool.size()==1)
        return *pool.begin();

    return nullptr;
}

BOOST_PYTHON_DECL PyTypeObject const* registration::to_python_target_type() const
{
    if (m_class_object)
        return m_class_object;

    if (m_to_python_target_type)
        return m_to_python_target_type();

    return nullptr;
}

BOOST_PYTHON_DECL PyTypeObject* registration::get_class_object() const
{
    if (!m_class_object) {
        PyErr_Format(PyExc_TypeError, "No Python class registered for C++ class %s",
                     target_type.pretty_name().c_str());
        throw_error_already_set();
    }
    
    return m_class_object;
}
  
BOOST_PYTHON_DECL PyObject* registration::to_python(void const* source) const
{
    if (!m_to_python) {
        PyErr_Format(PyExc_TypeError, "No to_python (by-value) converter found for C++ type: %s",
                     target_type.pretty_name().c_str());
        throw_error_already_set();
    }

    return (source == nullptr) ? python::detail::none() : m_to_python(source);
}


namespace // <unnamed>
{
  using registry_t = std::set<registration>;
  
  registry_t& entries()
  {
      static registry_t registry;

# ifndef BOOST_PYTHON_SUPPRESS_REGISTRY_INITIALIZATION
      static bool builtin_converters_initialized = false;
      if (!builtin_converters_initialized)
      {
          // Make this true early because registering the builtin
          // converters will cause recursion.
          builtin_converters_initialized = true;
          
          initialize_builtin_converters();
      }
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "registry: ";
      for (auto const& p : registry) {
          std::cout << p->target_type << "; ";
      }
      std::cout << '\n';
#  endif 
# endif 
      return registry;
  }

  registration& get_registration(type_info cpptype, bool is_shared_ptr = false)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      auto p = entries().find(registration(cpptype));
      std::cout << "looking up " << cpptype << ": "
                << (p == entries().end() || p->target_type != cpptype
                    ? "...NOT found\n" : "...found\n");
#  endif
      auto pos_ins = entries().emplace(cpptype, is_shared_ptr);
      return const_cast<registration&>(*pos_ins.first);
  }
} // namespace <unnamed>

namespace registry
{
  void insert(to_python_function to_python, type_info cpptype, pytype_function target_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting to_python " << cpptype << "\n";
#  endif 
      auto& slot = get_registration(cpptype);
      
      assert(slot.m_to_python == nullptr); // we have a problem otherwise
      if (slot.m_to_python) {
          auto msg = std::string("to-Python converter for ") + cpptype.pretty_name()
              + " already registered; second conversion method ignored.";
          
          if (PyErr_WarnEx(nullptr, msg.c_str(), 1))
              throw_error_already_set();
      }

      slot.m_to_python = to_python;
      slot.m_to_python_target_type = target_pytype;
  }

  // Insert an lvalue from_python converter
  void insert(convertible_function convertible, type_info cpptype, pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting lvalue from_python " << cpptype << "\n";
#  endif 
      auto& slot = get_registration(cpptype);
      slot.lvalue_chain.push_front({convertible});
      
      insert(convertible, nullptr, cpptype, exp_pytype);
  }

  // Insert an rvalue from_python converter
  void insert(convertible_function convertible, constructor_function construct,
              type_info cpptype, pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting rvalue from_python " << cpptype << "\n";
#  endif 
      auto& slot = get_registration(cpptype);
      slot.rvalue_chain.push_front({convertible, construct, exp_pytype});
  }

  // Insert an rvalue from_python converter
  void push_back(convertible_function convertible, constructor_function construct,
                 type_info cpptype, pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "push_back rvalue from_python " << cpptype << "\n";
#  endif 
      auto& slot = get_registration(cpptype);

      auto before_end = slot.rvalue_chain.before_begin();
      while (std::next(before_end) != slot.rvalue_chain.end())
          ++before_end;

      slot.rvalue_chain.insert_after(before_end, {convertible, construct, exp_pytype});
  }

  registration const& lookup(type_info cpptype, bool is_shared_ptr)
  {
      return get_registration(cpptype, is_shared_ptr);
  }

  registration const* query(type_info cpptype)
  {
      auto p = entries().find(registration(cpptype));
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "querying " << cpptype
                << (p == entries().end() || p->target_type != cpptype
                    ? "...NOT found\n" : "...found\n");
#  endif 
      return (p == entries().end() || p->target_type != cpptype) ? nullptr : &*p;
  }

  void set_class_object(type_info cpptype, PyTypeObject* class_object)
  {
      auto& slot = get_registration(cpptype);
      slot.m_class_object = class_object;
  }

} // namespace registry

}}} // namespace boost::python::converter
