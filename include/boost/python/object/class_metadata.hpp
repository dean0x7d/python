// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef CLASS_METADATA_DWA2004719_HPP
# define CLASS_METADATA_DWA2004719_HPP
# include <boost/python/converter/shared_ptr.hpp>

# include <boost/python/object/inheritance.hpp>
# include <boost/python/object/class_wrapper.hpp>
# include <boost/python/object/make_instance.hpp>
# include <boost/python/object/value_holder.hpp>
# include <boost/python/object/pointer_holder.hpp>
# include <boost/python/object/make_ptr_instance.hpp>

# include <boost/python/detail/unwrap_wrapper.hpp>

# include <boost/python/has_back_reference.hpp>
# include <boost/python/bases.hpp>

# include <boost/noncopyable.hpp>

namespace boost { namespace python { namespace objects { 

BOOST_PYTHON_DECL
void copy_class_object(type_info const& src, type_info const& dst);

//
// Support for registering base/derived relationships
//
template<class Derived, class BasesPack>
struct register_bases_of;

template<class Derived, class... Bases>
struct register_bases_of<Derived, bases<Bases...>> {
    static void execute() {
        // Use empty lambda to execute for all Bases
        [](...){}((register_base<Bases>(), 0)...);
    }

private:
    template<class Base>
    static void register_base() {
        static_assert(!std::is_same<Base, Derived>::value, "Base cannot be the same as Derived");

        // Register the Base class
        register_dynamic_id<Base>();
        // Register the up-cast
        register_conversion<Derived, Base>(false);
        // Register the down-cast, if appropriate.
        register_downcast<Base>(std::is_polymorphic<Base>{});
    }

    template<class Base>
    static void register_downcast(std::false_type) {}
    template<class Base>
    static void register_downcast(std::true_type) { register_conversion<Base, Derived>(true); }
};

//
// Preamble of register_class.  Also used for callback classes, which
// need some registration of their own.
//
template <class T, class Bases>
inline void register_shared_ptr_from_python_and_casts() {
    // Constructor performs registration
    converter::shared_ptr_from_python<T>{};

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
// Helpers for choosing the optional arguments
//
template<class T>
using is_bases = python::detail::is_<bases, T>;

template<class T>
using is_noncopyable = std::is_same<T, noncopyable>;

template<class T>
using is_held = std::integral_constant<bool, !is_bases<T>::value && !is_noncopyable<T>::value>;

// W is the class being wrapped. Args are arbitrarily-ordered optional arguments.
template<class W, class... Args>
struct class_metadata {
    // base classes of W
    using base_list = select_t<is_bases, bases<>, Args...>;
    // [a class derived from] W or a smart pointer to [a class derived from] W
    using held_type = select_t<is_held, W, Args...>;
    using is_noncopyable = any_t<is_noncopyable, Args...>;

private:
    // Hold by value if held_type is W or derived from W. (Otherwise held_type is a pointer.)
    using use_value_holder = std::is_base_of<W, held_type>;

    // Unwrap the held type if it's a pointer, i.e. get the pointee
    using unwapped_held_type = cpp14::conditional_t<
        use_value_holder::value,
        held_type,
        pointee_t<held_type>
    >;

    // Determine whether to use a "back-reference holder"
    template<class T>
    using is_self = std::is_same<W, T>;

    using use_back_reference = std::integral_constant<bool,
        has_back_reference<W>::value ||
        any_t<is_self, Args...>::value || // class_<W, W> has a back-reference
        (std::is_base_of<W, unwapped_held_type>::value &&
         !std::is_same<W, unwapped_held_type>::value)
    >;

public:
    using holder = cpp14::conditional_t<
        use_value_holder::value,
        value_holder<W, unwapped_held_type, use_back_reference::value>,
        pointer_holder<held_type, unwapped_held_type, W, use_back_reference::value>
    >;

    // Register the runtime metadata.
    static void register_() {
        register_aux((W*)nullptr);
    }

private:
    template<class T>
    static void register_aux(wrapper<T>*) {
        register_helper<T, !std::is_same<T, unwapped_held_type>::value>::execute();
    }

    static void register_aux(void*) {
        register_helper<W, std::is_base_of<W, unwapped_held_type>::value &&
                           !std::is_same<W, unwapped_held_type>::value>::execute();
    }

    template<class T, bool use_callback>
    struct register_helper {
        static void execute() {
            register_shared_ptr_from_python_and_casts<T, base_list>();

            maybe_register_callback_class(std::integral_constant<bool, use_callback>{});
            maybe_register_class_to_python(is_noncopyable{});
            maybe_register_pointer_to_python(use_back_reference{}, use_value_holder{});
        };

        // Support for registering callback classes
        static void maybe_register_callback_class(std::false_type /*use_callback*/) {}
        static void maybe_register_callback_class(std::true_type  /*use_callback*/) {
            register_shared_ptr_from_python_and_casts<unwapped_held_type, bases<T>>();
            copy_class_object(type_id<T>(), type_id<unwapped_held_type>());
        }

        // Support for registering to-python converters
        static void maybe_register_class_to_python(std::true_type  /*is_noncopyable*/) {}
        static void maybe_register_class_to_python(std::false_type /*is_noncopyable*/) {
            class_cref_wrapper<T, make_instance<T, holder>>{};
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
            copy_class_object(type_id<T>(), type_id<held_type>());
#endif
        }

        // Support for converting smart pointers to python
        static void maybe_register_pointer_to_python(...) {}

#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
        static void maybe_register_pointer_to_python(std::true_type /*use_back_reference*/,
                                                     ...            /*use_value_holder*/)
        {
            copy_class_object(type_id<W>(), type_id<back_reference<W const&>>());
            copy_class_object(type_id<W>(), type_id<back_reference<W&>>());
        }
#endif

        static void maybe_register_pointer_to_python(std::false_type /*use_back_reference*/,
                                                     std::false_type /*use_value_holder*/)
        {
            class_value_wrapper<
                held_type, make_ptr_instance<T, pointer_holder<held_type, T>>
            >();
#ifndef BOOST_PYTHON_NO_PY_SIGNATURES
            copy_class_object(type_id<T>(), type_id<held_type>());
#endif
        }
    };
};

}}} // namespace boost::python::object

#endif // CLASS_METADATA_DWA2004719_HPP
