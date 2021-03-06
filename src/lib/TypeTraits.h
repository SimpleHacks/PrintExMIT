/********************************************************************************
        Copyright 2015 Christopher Andrews.
        https://github.com/Chris--A/PrintEx

        This code mimics part of the C++11 STL library which provides
        meta-programming decision making. This is a minimal implementation
        and does not consider newer types such as nullptr_t and char32_t,
        which would be uncommon in basic Arduino applications.

        Add an issue to the tracker listed above to request these features.

        Released under MIT licence.
********************************************************************************/

#include "Globals.h"

#ifndef HEADER_TYPETRAITS
    #define HEADER_TYPETRAITS

    struct NullType{};

    /***********************************************************************************************
        select template.
            If V is true, type is T, otherwise U.
    ***********************************************************************************************/

    template< bool V, typename T, typename U > struct select { typedef U type; };
    template<typename T, typename U > struct select<true,T,U>{ typedef T type; };

    /***********************************************************************************************
        enable_if template.
            Similar to select, however if V is false, then type is undefined.

            Warning: Arduino IDE versions below 1.5.7 do not have GCC 4.8.1 which includes
            support for SFINAE. This template will not work in these IDE's.
            However you can use it to generate errors in C++98 when conditions aren't met.

            C++11: T can be omitted and `void` will be used.
    ***********************************************************************************************/

    #ifdef ISCPP11
        template< bool V, typename T = void > struct enable_if{};
    #else
        template< bool V, typename T > struct enable_if{}; //C++98 must provide all template parameters.
    #endif
    template< typename T > struct enable_if< true, T >{ typedef T type; };

    /***********************************************************************************************
        is_same structure.
            The template takes two types.
            If T is the same type as U value is true, otherwise false.
    ***********************************************************************************************/

    template < typename T, typename U > struct is_same{ enum { value = false }; };
    template < typename T > struct is_same< T, T >    { enum { value = true }; };

    /***********************************************************************************************
        is_integer structure.
            'value' will be set to true if t is a integer type ( 8 to 64 bit, signed / unsigned ).
            This is the compile time constant version of IsIntType().
    ***********************************************************************************************/

   /* template< typename T >
        struct is_integer{
            enum{
                V8    = is_same< T, uint8_t >::value  || is_same< T, int8_t >::value,
                V16   = is_same< T, uint16_t >::value || is_same< T, int16_t >::value,
                V32   = is_same< T, uint32_t >::value || is_same< T, int32_t >::value,
                V64   = is_same< T, uint64_t >::value || is_same< T, int64_t >::value,
                value = V8 || V16 || V32 || V64
            };
    };*/

    template< typename T >
        struct is_integer{
            enum{
                V8    = is_same< T, char >::value || is_same< T, unsigned char >::value  || is_same< T, signed char >::value,
                V16   = is_same< T, unsigned int >::value || is_same< T, signed int >::value,
                V32   = is_same< T, unsigned long >::value || is_same< T, signed long >::value,
                V64   = is_same< T, unsigned long long >::value || is_same< T, signed long long >::value,
                value = V8 || V16 || V32 || V64
            };
    };

    template< typename T > struct is_float{ enum{ value = is_same< T, float >::value || is_same< T, double >::value || is_same< T, long double >::value }; };
    template< typename T > struct is_number{ enum{ value = is_float< T >::value || is_integer< T >::value }; };
    template< typename T > struct is_bool{ enum{ value = is_same< T, bool >::value }; };
    template< typename T > struct is_fundamental{ enum{ value = is_number<T>::value || is_bool<T>::value }; };

    /*** is_pointer can allow generic template functions to discriminate based on whether a template type is a pointer. ***/
    template< typename T > struct is_pointer{ enum{ value = false }; };      //Not a pointer.
    template< typename T > struct is_pointer<T*>{ enum{ value = true }; };   //A standard pointer.
    template< typename T > struct is_pointer<T*&>{ enum{ value = true }; };  //A reference to a pointer (is still considered a pointer in usage due to reference semantics).


    /* *************************************************************************************
        is_array structure.
            Useful to disambiguate T *t, and T(&t)[N] situations.
            Switch use over to ArrayInfo as it exposes more information.
    ************************************************************************************* */

    template< typename T > struct is_array{ enum { value = false }; };
    template< typename T, size_t N > struct is_array< T[N] > { enum { value = true }; };
    template< typename T, size_t A, size_t N > struct is_array< T[A][N] > { enum { value = true }; };
    template< typename T, size_t B, size_t A, size_t N > struct is_array< T[B][A][N] > { enum { value = true }; };

    /* *************************************************************************************
        ArrayInfo structure.
            A useful way to disambiguate multi dimension array's.
            Also T *t, and T(&t)[N] situations can be solved using IsAnArray value.
    ************************************************************************************* */

    template< typename T >                             /*--- 'Not an array' specialisation. ---*/
        struct ArrayInfo{
            enum{
                IsAnArray = false,
                First = 0x00,
                Dimensions = 0x00,
            };

            typedef T            ThisType;
            typedef NullType    NextType;
            typedef ThisType    UnderlyingType;
    };

    template< typename T, size_t N >
        struct ArrayInfo< T[N] >{                     /*--- Single dimension specialisation. ---*/
            enum{
                IsAnArray = true,
                First = N,
                Dimensions = 0x01,
            };
            static const size_t Indicies[ Dimensions ];
            typedef T ThisType[N];
            typedef T NextType;
            typedef T UnderlyingType;
    };

    template< typename T, size_t A, size_t N >
        struct ArrayInfo< T[A][N] >{                /*--- Two dimensional array specialisation. ---*/
            enum{
                IsAnArray = true,
                First = A,
                Second = N,
                Dimensions = 0x02,
            };
            static const size_t Indicies[ Dimensions ];
            typedef T ThisType[A][N];
            typedef T NextType[N];
            typedef T UnderlyingType;
    };

    template< typename T, size_t B, size_t A, size_t N >
        struct ArrayInfo< T[B][A][N] >{             /*--- Three dimensional array specialisation. ---*/
            enum{
                IsAnArray = true,
                First = B,
                Second = A,
                Third = N,
                Dimensions = 0x03,
            };
            static const size_t Indicies[ Dimensions ];
            typedef T ThisType[B][A][N];
            typedef T NextType[A][N];
            typedef T UnderlyingType;
    };

    template< typename T, size_t C, size_t B, size_t A, size_t N >
        struct ArrayInfo< T[C][B][A][N] >{            /*--- Four dimensional array specialisation. ---*/
            enum{
                IsAnArray = true,
                First = C,
                Second = B,
                Third = A,
                Fourth = N,
                Dimensions = 0x04,
            };
            static const size_t Indicies[ Dimensions ];
            typedef T ThisType[C][B][A][N];
            typedef T NextType[B][A][N];
            typedef T UnderlyingType;
    };
    /*
    template< typename T >
        const size_t ArrayInfo< T >::Indicies[ ArrayInfo< T >::Dimensions ] = {
            ArrayInfo< T >::First
    };*/

    template< typename T, size_t N >
        const size_t ArrayInfo< T[N] >::Indicies[ ArrayInfo< T[N] >::Dimensions ] = {
            ArrayInfo< T[N] >::First
    };

    template< typename T, size_t A, size_t N >
        const size_t ArrayInfo< T[A][N] >::Indicies[ ArrayInfo< T[A][N] >::Dimensions ] = {
            ArrayInfo< T[A][N] >::First,
            ArrayInfo< T[A][N] >::Second
    };

    template< typename T, size_t B, size_t A, size_t N >
        const size_t ArrayInfo< T[B][A][N] >::Indicies[ ArrayInfo< T[B][A][N] >::Dimensions ] = {
            ArrayInfo< T[B][A][N] >::First,
            ArrayInfo< T[B][A][N] >::Second,
            ArrayInfo< T[B][A][N] >::Third
    };

    template< typename T, size_t C, size_t B, size_t A, size_t N >
        const size_t ArrayInfo< T[C][B][A][N] >::Indicies[ ArrayInfo< T[C][B][A][N] >::Dimensions ] = {
            ArrayInfo< T[C][B][A][N] >::First,
            ArrayInfo< T[C][B][A][N] >::Second,
            ArrayInfo< T[C][B][A][N] >::Third,
            ArrayInfo< T[C][B][A][N] >::Fourth,
    };


    /* *************************************************************************************
        is_base_of structure.
            Determines whether D is inherited by B.
    ************************************************************************************* */


    #ifdef ISCPP11
        template<class B, class D, typename C = void> struct is_base_of;

        template<class B, class D>
            struct is_base_of<B,D,typename enable_if<
                !is_array<D>::value &&
                !is_fundamental<D>::value &&
                !is_pointer<D>::value>::type
            >{
              template< typename T > struct dummy{};
              struct Child : D, dummy<int> {};
              static B* Check (B*);
              template< typename T > static char Check (dummy<T>*);
              static const bool value = (sizeof(Check((Child*)0)) == sizeof(B*));
        };

        template<class B, class D>
            struct is_base_of<B,D,typename enable_if<
                is_array<D>::value ||
                is_fundamental<D>::value ||
                is_pointer<D>::value>::type
            >{
            static const bool value = 0;
        };
    #endif
#endif
