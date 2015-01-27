//  Copyright David Abrahams 2001.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/converter/registry.hpp>
#include <boost/python/converter/registrations.hpp>
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
    for (const auto& rvalue_converter : rvalue_chain) {
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
    if (this->m_class_object != 0)
        return this->m_class_object;

    if (this->m_to_python_target_type != 0)
        return this->m_to_python_target_type();

    return 0;
}

BOOST_PYTHON_DECL PyTypeObject* registration::get_class_object() const
{
    if (this->m_class_object == 0)
    {
        ::PyErr_Format(
            PyExc_TypeError
            , const_cast<char*>("No Python class registered for C++ class %s")
            , this->target_type.name());
    
        throw_error_already_set();
    }
    
    return this->m_class_object;
}
  
BOOST_PYTHON_DECL PyObject* registration::to_python(void const volatile* source) const
{
    if (!m_to_python) {
        handle<> msg(
#if PY_VERSION_HEX >= 0x3000000
            ::PyUnicode_FromFormat
#else
            ::PyString_FromFormat
#endif
            ("No to_python (by-value) converter found for C++ type: %s", target_type.name())
        );

        PyErr_SetObject(PyExc_TypeError, msg.get());
        throw_error_already_set();
    }
        
    return (source == nullptr)
        ? incref(Py_None)
        : m_to_python(const_cast<void*>(source));
}


namespace // <unnamed>
{
  typedef std::set<registration> registry_t;
  
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
      for (registry_t::iterator p = registry.begin(); p != registry.end(); ++p)
      {
          std::cout << p->target_type << "; ";
      }
      std::cout << '\n';
#  endif 
# endif 
      return registry;
  }

  registration* get(type_info type, bool is_shared_ptr = false)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      registry_t::iterator p = entries().find(registration(type));
      
      std::cout << "looking up " << type << ": "
                << (p == entries().end() || p->target_type != type
                    ? "...NOT found\n" : "...found\n");
#  endif
      auto pos_ins = entries().emplace(type, is_shared_ptr);
      
      return const_cast<registration*>(&*pos_ins.first);
  }
} // namespace <unnamed>

namespace registry
{
  void insert(to_python_function_t f, type_info source_t, pytype_function to_python_target_type)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting to_python " << source_t << "\n";
#  endif 
      registration* slot = get(source_t);
      
      assert(slot->m_to_python == 0); // we have a problem otherwise
      if (slot->m_to_python != 0)
      {
          std::string msg = (
              std::string("to-Python converter for ")
              + source_t.name()
              + " already registered; second conversion method ignored."
          );
          
          if ( ::PyErr_Warn( NULL, const_cast<char*>(msg.c_str()) ) )
          {
              throw_error_already_set();
          }
      }
      slot->m_to_python = f;
      slot->m_to_python_target_type = to_python_target_type;
  }

  // Insert an lvalue from_python converter
  void insert(convertible_function convert, type_info key, pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting lvalue from_python " << key << "\n";
#  endif 
      registration* found = get(key);
      found->lvalue_chain.push_front({convert});
      
      insert(convert, 0, key,exp_pytype);
  }

  // Insert an rvalue from_python converter
  void insert(convertible_function convertible
              , constructor_function construct
              , type_info key
              , pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "inserting rvalue from_python " << key << "\n";
#  endif 
      registration* found = get(key);
      found->rvalue_chain.push_front({convertible, construct, exp_pytype});
  }

  // Insert an rvalue from_python converter
  void push_back(convertible_function convertible
              , constructor_function construct
              , type_info key
              , pytype_function exp_pytype)
  {
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "push_back rvalue from_python " << key << "\n";
#  endif 
      registration* found = get(key);
      auto before_end = found->rvalue_chain.before_begin();
      while (std::next(before_end) != found->rvalue_chain.end())
          ++before_end;

      found->rvalue_chain.insert_after(before_end, {convertible, construct, exp_pytype});
  }

  registration const& lookup(type_info key)
  {
      return *get(key);
  }

  registration const& lookup_shared_ptr(type_info key)
  {
      return *get(key, true);
  }

  registration const* query(type_info type)
  {
      registry_t::iterator p = entries().find(registration(type));
#  ifdef BOOST_PYTHON_TRACE_REGISTRY
      std::cout << "querying " << type
                << (p == entries().end() || p->target_type != type
                    ? "...NOT found\n" : "...found\n");
#  endif 
      return (p == entries().end() || p->target_type != type) ? 0 : &*p;
  }
} // namespace registry

}}} // namespace boost::python::converter
