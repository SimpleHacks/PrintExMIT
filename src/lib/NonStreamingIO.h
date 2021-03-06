/********************************************************************************
    Copyright 2015 Christopher Andrews.
    https://github.com/Chris--A/PrintEx

    Released under MIT licence.
********************************************************************************/

#include "Globals.h"
#include "Arduino.h"

#ifndef HEADER_NONSTREAMINGIO
    #define HEADER_NONSTREAMINGIO

    /***************************************************************************
        NonStreamingIO Class.
            This interface provides an extension to the Print class.
            It allows IO capabilities for derived objects that may not
            be streams. As in, the data printed to the object is still
            available for use.

            This class does not directly do any reading, it provides a start
            and end pointer of data written, which can then be used by Print
            to re-print the data.
    ***************************************************************************/

    class NonStreamingIO{
        public:

            size_t count( void ) const { return len; }

            NonStreamingIO &operator =( uint8_t *t )
                {
                    start = t;
                    len = 0;
                    return *this;
                }

            template< typename T >
                inline void assign( const T *data, const uint16_t length )
                    {
                        start = ( uint8_t* ) data;
                        len = length;
                    }

            uint8_t *start;
            uint16_t len;

        protected:
            CONSTEXPR NonStreamingIO( uint8_t *data ) : start( data ) , len( 0 )                                 {}
            CONSTEXPR NonStreamingIO( uint8_t *data, const unsigned int length ) : start( data ) , len( length ) {}
        private:
    };

#endif
