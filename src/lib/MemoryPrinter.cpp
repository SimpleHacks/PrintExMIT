/********************************************************************************
    Copyright 2015 Christopher Andrews.
    https://github.com/Chris--A/PrintEx

    Released under MIT licence.
********************************************************************************/

    #include "MemoryPrinter.h"


    /********************************************************************************
        EEPROMPrinter class
    ********************************************************************************/
#ifdef __AVR__
    size_t EEPROMPrinter::write( uint8_t u_Data )
        {
            if( eeprom_read_byte( start + len ) != u_Data )
                eeprom_write_byte( start + len, u_Data );
            ++len;
            return 0x01;
        }

    size_t EEPROMPrinter::printTo(Print& p) const
        {
            for( uint16_t counter = 0 ; counter < len ; ++counter ){
                p.write( ( uint8_t ) eeprom_read_byte( &start[counter] ) );
            }
            return len;
        }
#endif
    /********************************************************************************
        SRAMPrinter class
    ********************************************************************************/

    size_t SRAMPrinter::write( uint8_t data )
        {
            start[len++] = data;
            return 0x01;
        }

    /********************************************************************************
        PROGMEMPrinter class
    ********************************************************************************/
#ifdef __AVR__
    size_t PROGMEMPrinter::printTo(Print& p) const
        {
            size_t returnSize = 0;
            for( uint16_t counter = 0 ;; ++counter ){
                unsigned char current = pgm_read_byte( &start[counter] );
                if ( !current ) break;
                returnSize += p.write( current );
            }
            return returnSize;
        }
#endif
