// Copyright David Abrahams 2004. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <boost/python/converter/arg_from_python.hpp>
#include <boost/python/type_id.hpp>
#include <iostream>

int result;

#define ASSERT_SAME(T1,T2) \
       if (!std::is_same< T1, T2 >::value) { \
             std::cout << "*********************\n"; \
             std::cout << python::type_id< T1 >() << " != " << python::type_id< T2 >() << "\n"; \
             std::cout << "*********************\n"; \
             result = 1; \
       }

int main()
{
    using namespace boost::python::converter;
    using namespace boost;


    ASSERT_SAME(
        select_arg_from_python<int>::type, arg_rvalue_from_python<int>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const>::type, arg_rvalue_from_python<int const>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile>::type, arg_rvalue_from_python<int volatile>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile>::type, arg_rvalue_from_python<int const volatile>
        );



    ASSERT_SAME(
        select_arg_from_python<int*>::type, pointer_arg_from_python<int*>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*>::type, pointer_arg_from_python<int const*>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*>::type, pointer_arg_from_python<int volatile*>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*>::type, pointer_arg_from_python<int const volatile*>
        );




    ASSERT_SAME(
        select_arg_from_python<int&>::type, reference_arg_from_python<int&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const&>::type, arg_rvalue_from_python<int const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile&>::type, reference_arg_from_python<int volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile&>::type, reference_arg_from_python<int const volatile&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*&>::type, reference_arg_from_python<int*&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*&>::type, reference_arg_from_python<int const*&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*&>::type, reference_arg_from_python<int volatile*&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*&>::type, reference_arg_from_python<int const volatile*&>
        );



    ASSERT_SAME(
        select_arg_from_python<int* const&>::type, pointer_arg_from_python<int*const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const* const&>::type, pointer_arg_from_python<int const*const&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile* const&>::type, pointer_arg_from_python<int volatile*const&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile* const&>::type, pointer_arg_from_python<int const volatile*const&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*volatile&>::type, reference_arg_from_python<int*volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*volatile&>::type, reference_arg_from_python<int const*volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*volatile&>::type, reference_arg_from_python<int volatile*volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*volatile&>::type, reference_arg_from_python<int const volatile*volatile&>
        );



    ASSERT_SAME(
        select_arg_from_python<int*const volatile&>::type, reference_arg_from_python<int*const volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int const*const volatile&>::type, reference_arg_from_python<int const*const volatile&>
        );
    
    ASSERT_SAME(
        select_arg_from_python<int volatile*const volatile&>::type, reference_arg_from_python<int volatile*const volatile&>
        );

    ASSERT_SAME(
        select_arg_from_python<int const volatile*const volatile&>::type, reference_arg_from_python<int const volatile*const volatile&>
        );
    return result;
}
