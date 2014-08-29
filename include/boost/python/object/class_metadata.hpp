// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CLASS_METADATA_DWA2004719_HPP
# define CLASS_METADATA_DWA2004719_HPP
# include <boost/python/converter/shared_ptr_from_python.hpp>

# include <boost/python/object/inheritance.hpp>
# include <boost/python/object/class_wrapper.hpp>
# include <boost/python/object/make_instance.hpp>
# include <boost/python/object/value_holder.hpp>
# include <boost/python/object/pointer_holder.hpp>
# include <boost/python/object/make_ptr_instance.hpp>

# include <boost/python/detail/force_instantiate.hpp>
# include <boost/python/detail/not_specified.hpp>

# include <boost/python/has_back_reference.hpp>
# include <boost/python/bases.hpp>

# include <boost/noncopyable.hpp>

namespace boost { namespace python { namespace objects { 

BOOST_PYTHON_DECL
void copy_class_object(type_info const& src, type_info const& dst);

//
// Support for registering base/derived relationships
//
template <class Derived, class BasesPack>
struct register_bases_of;

template <class Derived, class Base, class... Tail>
struct register_bases_of<Derived, bases<Base, Tail...>>
{
    static void execute()
    {
        static_assert(!std::is_same<Base, Derived>::value, 
                      "Base cannot be the same as Derived");
        
        // Register the Base class
        register_dynamic_id<Base>();

        // Register the up-cast
        register_conversion<Derived, Base>(false);

        // Register the down-cast, if appropriate.
        register_downcast(std::is_polymorphic<Base>());
        
        // Repeat for the rest of the bases
        register_bases_of<Derived, bases<Tail...>>::execute();
    }

 private:
    static inline void register_downcast(std::false_type) {}
    
    static inline void register_downcast(std::true_type)
    {
        register_conversion<Base, Derived>(true);
    }
};

template <class Derived>
struct register_bases_of<Derived, bases<>>
{
    static void execute() {}
};

//
// Preamble of register_class.  Also used for callback classes, which
// need some registration of their own.
//
template <class T, class Bases>
inline void register_shared_ptr_from_python_and_casts(T*, Bases)
{
    // Constructor performs registration
    python::detail::force_instantiate(converter::shared_ptr_from_python<T>());

    // Register all up/downcasts here
    register_dynamic_id<T>();
    register_bases_of<T, Bases>::execute();
}
  
namespace detail {
    template<template<class> class Predicate, class Default, class... Args>
    struct select;
    
    template<template<class> class Predicate, class Default>
    struct select<Predicate, Default> {
        using type = Default;
    };
    
    template<template<class> class Predicate, class Default, class T, class... Tail>
    struct select<Predicate, Default, T, Tail...> {
        using type = cpp14::conditional_t<
            Predicate<T>::value,
            T,
            typename select<Predicate, Default, Tail...>::type
        >;
    };
    
    template<template<class> class Predicate, class... Args>
    struct any;
    
    template<template<class> class Predicate>
    struct any<Predicate> {
        static constexpr bool value = false;
    };
    
    template<template<class> class Predicate, class T, class... Tail>
    struct any<Predicate, T, Tail...> {
        static constexpr bool value = Predicate<T>::value || any<Predicate, Tail...>::value;
    };
}

// Select the Arg that satisfies the Predicate or return Default if none do.
template<template<class> class Predicate, class Default, class... Args>
using select_t = typename detail::select<Predicate, Default, Args...>::type;

// Does any Arg satisfy the Predicate?
template<template<class> class Predicate, class... Args>
using any_t = std::integral_constant<bool, detail::any<Predicate, Args...>::value>;

//
// Helper for choosing the unnamed held_type argument
//
template<class T>
using is_held_type = std::integral_constant<bool, 
    !python::detail::specifies_bases<T>::value && !std::is_same<T, noncopyable>::value
>;

template<class T>
using is_noncopyable_type = std::integral_constant<bool, 
    std::is_same<T, noncopyable>::value
>;

template <
    class T // class being wrapped
  , class X1 // = detail::not_specified
  , class X2 // = detail::not_specified
  , class X3 // = detail::not_specified
>
struct class_metadata
{
    //
    // Calculate the unnamed template arguments
    //
    
    // held_type_arg -- not_specified, [a class derived from] T or a
    // smart pointer to [a class derived from] T.  Preserving
    // not_specified allows us to give class_<T,T> a back-reference.
    using held_type_arg = select_t<is_held_type, python::detail::not_specified, X1, X2, X3>;

    // bases
    using bases = select_t<python::detail::specifies_bases, python::bases<>, X1, X2, X3>;

    using is_noncopyable = any_t<is_noncopyable_type, X1, X2, X3>;
    
    //
    // Holder computation.
    //
    
    // Compute the actual type that will be held in the Holder.
    using held_type = cpp14::conditional_t<
        std::is_same<held_type_arg, python::detail::not_specified>::value, T, held_type_arg
    >;

    // Determine if the object will be held by value
    using use_value_holder = std::is_convertible<held_type*, T*>;
    
    // Compute the "wrapped type", that is, if held_type is a smart
    // pointer, we're talking about the pointee.
    using wrapped = pointee_t<held_type, !use_value_holder::value>;

    // Determine whether to use a "back-reference holder"
    using use_back_reference = std::integral_constant<bool,
        has_back_reference<T>::value || 
        std::is_same<held_type_arg, T>::value || 
        (std::is_base_of<T, wrapped>::value && !std::is_same<T, wrapped>::value)
    >;

    // Select the holder.
    using holder = cpp14::conditional_t<
        use_back_reference::value,
        cpp14::conditional_t<
            use_value_holder::value,
            value_holder_back_reference<T, wrapped>,
            pointer_holder_back_reference<held_type, T>
        >,
        cpp14::conditional_t<
            use_value_holder::value,
            value_holder<T>,
            pointer_holder<held_type, wrapped>
        >
    >;
    
    inline static void register_() // Register the runtime metadata.
    {
        class_metadata::register_aux((T*)0);
    }

 private:
    template <class T2>
    inline static void register_aux(python::wrapper<T2>*) 
    {
        class_metadata::register_aux2<T2, !std::is_same<T2, wrapped>::value>();
    }

    inline static void register_aux(void*) 
    {
        class_metadata::register_aux2<
            T, 
            std::is_base_of<T, wrapped>::value && !std::is_same<T, wrapped>::value
        >();
    }

    template <class T2, bool use_callback>
    inline static void register_aux2() 
    {
        objects::register_shared_ptr_from_python_and_casts((T2*)0, bases());
        
        class_metadata::maybe_register_callback_class((T2*)0, 
            std::integral_constant<bool, use_callback>());

        class_metadata::maybe_register_class_to_python((T2*)0, is_noncopyable());
        
        class_metadata::maybe_register_pointer_to_python(
            (T2*)0, (use_value_holder*)0, (use_back_reference*)0);
    }


    //
    // Support for converting smart pointers to python
    //
    inline static void maybe_register_pointer_to_python(...) {}

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
    inline static void maybe_register_pointer_to_python(void*,void*,std::true_type*) 
    {
        objects::copy_class_object(python::type_id<T>(), python::type_id<back_reference<T const &> >());
        objects::copy_class_object(python::type_id<T>(), python::type_id<back_reference<T &> >());
    }
#endif

    template <class T2>
    inline static void maybe_register_pointer_to_python(T2*, std::false_type*, std::false_type*)
    {
        python::detail::force_instantiate(
            objects::class_value_wrapper<
                held_type
              , make_ptr_instance<T2, pointer_holder<held_type, T2> >
            >()
        );
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        // explicit qualification of type_id makes msvc6 happy
        objects::copy_class_object(python::type_id<T2>(), python::type_id<held_type>());
#endif
    }
    //
    // Support for registering to-python converters
    //
    inline static void maybe_register_class_to_python(void*, std::true_type) {}
    

    template <class T2>
    inline static void maybe_register_class_to_python(T2*, std::false_type)
    {
        python::detail::force_instantiate(class_cref_wrapper<T2, make_instance<T2, holder> >());
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        // explicit qualification of type_id makes msvc6 happy
        objects::copy_class_object(python::type_id<T2>(), python::type_id<held_type>());
#endif
    }

    //
    // Support for registering callback classes
    //
    inline static void maybe_register_callback_class(void*, std::false_type) {}

    template <class T2>
    inline static void maybe_register_callback_class(T2*, std::true_type)
    {
        objects::register_shared_ptr_from_python_and_casts(
            (wrapped*)0, python::bases<T2>());

        // explicit qualification of type_id makes msvc6 happy
        objects::copy_class_object(python::type_id<T2>(), python::type_id<wrapped>());
    }
};

}}} // namespace boost::python::object

#endif // CLASS_METADATA_DWA2004719_HPP
