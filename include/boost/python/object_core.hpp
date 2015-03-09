// Copyright David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef OBJECT_CORE_DWA2002615_HPP
# define OBJECT_CORE_DWA2002615_HPP

# include <boost/python/detail/prefix.hpp>

# include <boost/python/call.hpp>
# include <boost/python/handle_fwd.hpp>
# include <boost/python/errors.hpp>
# include <boost/python/refcount.hpp>
# include <boost/python/def_visitor.hpp>

# include <boost/python/detail/raw_pyobject.hpp>
# include <boost/python/detail/dependent.hpp>

# include <boost/python/object/add_to_namespace.hpp>

# include <boost/python/detail/is_xxx.hpp>
# include <boost/python/detail/string_literal.hpp>
# include <boost/python/detail/def_helper_fwd.hpp>
# include <boost/python/detail/unwrap.hpp>

namespace boost { namespace python { 

namespace detail
{
  class kwds_proxy; 
  class args_proxy; 
} 

namespace converter
{
  template <class T> struct arg_to_python;
}

// Put this in an inner namespace so that the generalized operators won't take over
namespace api
{
  
// This file contains the definition of the object class and enough to
// construct/copy it, but not enough to do operations like
// attribute/item access or addition.

  template <class Policies> class proxy;
  
  struct const_attribute_policies;
  struct attribute_policies;
  struct const_objattribute_policies;
  struct objattribute_policies;
  struct const_item_policies;
  struct item_policies;
  struct const_slice_policies;
  struct slice_policies;
  class slice_nil;

  typedef proxy<const_attribute_policies> const_object_attribute;
  typedef proxy<attribute_policies> object_attribute;
  typedef proxy<const_objattribute_policies> const_object_objattribute;
  typedef proxy<objattribute_policies> object_objattribute;
  typedef proxy<const_item_policies> const_object_item;
  typedef proxy<item_policies> object_item;
  typedef proxy<const_slice_policies> const_object_slice;
  typedef proxy<slice_policies> object_slice;

  class object;

  template <class U>
  class object_operators : public def_visitor<U> {
      using bool_type = PyObject* (object::*)() const;
   protected:
      typedef object const& object_cref;
   public:
      // function call
      //
      object operator()() const;

      template <typename... Args>
      object operator()(Args const&... args) const;

      detail::args_proxy operator* () const; 
      object operator()(detail::args_proxy const &args) const; 
      object operator()(detail::args_proxy const &args, 
                        detail::kwds_proxy const &kwds) const; 

      // truth value testing
      //
      operator bool_type() const;
      bool operator!() const; // needed for vc6

      // Attribute access
      //
      const_object_attribute attr(char const*) const;
      object_attribute attr(char const*);
      const_object_objattribute attr(object const&) const;
      object_objattribute attr(object const&);

      // Wrap 'in' operator (aka. __contains__)
      template <class T>
      object contains(T const& key) const;
      
      // item access
      //
      const_object_item operator[](object_cref) const;
      object_item operator[](object_cref);
    
      template <class T>
      const_object_item
      operator[](T const& key) const;
    
      template <class T>
      object_item
      operator[](T const& key);

      // slicing
      //
      const_object_slice slice(object_cref, object_cref) const;
      object_slice slice(object_cref, object_cref);

      const_object_slice slice(slice_nil, object_cref) const;
      object_slice slice(slice_nil, object_cref);
                             
      const_object_slice slice(object_cref, slice_nil) const;
      object_slice slice(object_cref, slice_nil);

      const_object_slice slice(slice_nil, slice_nil) const;
      object_slice slice(slice_nil, slice_nil);

      template <class T, class V>
      const_object_slice
      slice(T const& start, V const& end) const;
    
      template <class T, class V>
      object_slice
      slice(T const& start, V const& end);
      
   private: // def visitation for adding callable objects as class methods
      
      template <class ClassT, class DocStringT>
      void visit(ClassT& cl, char const* name, python::detail::def_helper<DocStringT> const& helper) const
      {
          // It's too late to specify anything other than docstrings if
          // the callable object is already wrapped.
          static_assert(
              std::is_same<char const*, DocStringT>::value ||
              detail::is_string_literal<DocStringT const>::value, ""
          );
        
          objects::add_to_namespace(cl, name, this->derived_visitor(), helper.doc());
      }

      friend class python::def_visitor_access;
      
   private:
     // there is a confirmed CWPro8 codegen bug here. We prevent the
     // early destruction of a temporary by binding a named object
     // instead.
    typedef object const object_cref2;
  };

  template <class T>
  PyObject* object_initializer(T const& x);
  
  class object : public object_operators<object> {
  public:
      // default constructor creates a None object
      object() : m_ptr{python::detail::none()} {}
      
      // explicit conversion from any C++ object to Python
      template <class T>
      explicit object(T const& x) : m_ptr{object_initializer(x)} {}

      // Throw error_already_set() if the handle is null.
      explicit object(handle<> const& x)
          : m_ptr{python::incref(python::expect_non_null(x.get()))}
      {}

      // copy constructor without NULL checking, for efficiency.
      object(object const& rhs) : m_ptr{python::incref(rhs.m_ptr)} {}

      object& operator=(object const& rhs) {
          python::incref(rhs.m_ptr);
          python::decref(this->m_ptr);
          this->m_ptr = rhs.m_ptr;
          return *this;
      }

      ~object() { python::decref(m_ptr); }

      // Underlying object access -- returns a borrowed reference
      PyObject* ptr() const { return m_ptr; }

      bool is_none() const { return m_ptr == Py_None; }

  private:
      PyObject* m_ptr;

  public: // implementation detail -- for internal use only
      explicit object(detail::borrowed_reference p) : m_ptr{python::incref((PyObject*)p)} {}
      explicit object(detail::new_reference p) : m_ptr{expect_non_null((PyObject*)p)} {}
      explicit object(detail::new_non_null_reference p) : m_ptr{(PyObject*)p} {}
  };

  // Macros for forwarding constructors in classes derived from
  // object. Derived classes will usually want these as an
  // implementation detail
# define BOOST_PYTHON_FORWARD_OBJECT_CONSTRUCTORS(derived, base)               \
    inline explicit derived(::boost::python::detail::borrowed_reference p)     \
        : base(p) {}                                                           \
    inline explicit derived(::boost::python::detail::new_reference p)          \
        : base(p) {}                                                           \
    inline explicit derived(::boost::python::detail::new_non_null_reference p) \
        : base(p) {}

  //
  // object_initializer -- get the handle to construct the object with,
  // based on whether T is a proxy or derived from object
  //
  template <bool is_proxy = false, bool is_object_manager = false>
  struct object_initializer_impl {
      static PyObject* get(object const& x, std::true_type) {
          return python::incref(x.ptr());
      }
      
      template <class T>
      static PyObject* get(T const& x, std::false_type) {
          return python::incref(converter::arg_to_python<T>(x).get());
      }
  };
      
  template <>
  struct object_initializer_impl<true, false> {
      template <class Policies>
      static PyObject* get(proxy<Policies> const& x, std::false_type) {
          return python::incref(x.operator object().ptr());
      }
  };

  template <>
  struct object_initializer_impl<false, true> {
      template <class T, class U>
      static PyObject* get(T const& x, U) {
          return python::incref(get_managed_object(x));
      }
  };

  template <>
  struct object_initializer_impl<true, true>
  {}; // empty implementation should cause an error

  template <class T>
  PyObject* object_initializer(T const& x) {
      using type = cpp14::conditional_t<
          python::detail::is_<std::reference_wrapper, T>::value,
          detail::unwrap_t<T>,
          T
      >;
      return object_initializer_impl<
          detail::is_<boost::python::api::proxy, type>::value,
          converter::is_object_manager<type>::value
      >::get(x, std::is_base_of<object, T>());
  }

} // namespace api
using api::object;
template <class T> struct extract;

//
// implementation
//

namespace detail 
{

class call_proxy 
{ 
public: 
  call_proxy(object target) : m_target(target) {} 
  operator object() const { return m_target;} 
 
 private: 
    object m_target; 
}; 
 
class kwds_proxy : public call_proxy 
{ 
public: 
  kwds_proxy(object o = object()) : call_proxy(o) {} 
}; 
class args_proxy : public call_proxy 
{ 
public: 
  args_proxy(object o) : call_proxy(o) {} 
  kwds_proxy operator* () const { return kwds_proxy(*this);} 
}; 
} 

template <typename U>
template <typename... Args>
object api::object_operators<U>::operator()(Args const&... args) const
{
    // TODO: this should use perfect forwarding, but the converter isn't compatible yet
    U const& self = *static_cast<U const*>(this);
    return call<object>(get_managed_object(self), args...);
}

template <typename U> 
detail::args_proxy api::object_operators<U>::operator* () const 
{ 
  object_cref2 x = *static_cast<U const*>(this); 
  return boost::python::detail::args_proxy(x); 
} 
 
template <typename U> 
object api::object_operators<U>::operator()(detail::args_proxy const &args) const 
{ 
  U const& self = *static_cast<U const*>(this); 
  PyObject *result = PyObject_Call(get_managed_object(self),
                                   args.operator object().ptr(), 
                                   0); 
  return object(boost::python::detail::new_reference(result)); 
 
} 
 
template <typename U> 
object api::object_operators<U>::operator()(detail::args_proxy const &args, 
                                            detail::kwds_proxy const &kwds) const 
{ 
  U const& self = *static_cast<U const*>(this); 
  PyObject *result = PyObject_Call(get_managed_object(self),
                                   args.operator object().ptr(), 
                                   kwds.operator object().ptr()); 
  return object(boost::python::detail::new_reference(result)); 
 
}  


template <typename U>
template <class T>
object api::object_operators<U>::contains(T const& key) const
{
    return this->attr("__contains__")(object(key));
}


//
// Converter specialization implementations
//
namespace converter
{
  template <class T> struct object_manager_traits;
  
  template <>
  struct object_manager_traits<object>
  {
      static constexpr bool is_specialized = true;
      static bool check(PyObject*) { return true; }
      
      static python::detail::new_non_null_reference adopt(PyObject* x)
      {
          return python::detail::new_non_null_reference(x);
      }
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
      static PyTypeObject const *get_pytype() {return 0;}
#endif
  };
}

inline PyObject* get_managed_object(object const& x)
{
    return x.ptr();
}

}} // namespace boost::python

# include <boost/python/slice_nil.hpp>

#endif // OBJECT_CORE_DWA2002615_HPP
