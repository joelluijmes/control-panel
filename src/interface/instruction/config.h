#pragma once
#include <avr/io.h>

#define F_CPU               8000000

#define MAX7221_DEVICES     2

#define MAX7221_DATA_DDR    DDRB
#define MAX7221_DATA_PORT   PORTB
#define MAX7221_DATA_MASK   (1 << 7)

#define MAX7221_CS_DDR      DDRD
#define MAX7221_CS_PORT     PORTD
#define MAX7221_CS_MASK     (1 << 0 | 1 << 1)

#define MAX7221_SCK_DDR     DDRD
#define MAX7221_SCK_PORT    PORTD
#define MAX7221_SCK_MASK    (1 <<5)

#define STP16_CLK_DDR       DDRC      
#define STP16_CLK_PORT      PORTC
#define STP16_CLK_MASK      1 << 0

#define STP16_DATA_DDR      DDRC
#define STP16_DATA_PORT     PORTC
#define STP16_DATA_MASK     1 << 1

#define STP16_LE_DDR        DDRA
#define STP16_LE_PORT       PORTA
#define STP16_LE_MASK       1 << 1

#define STP16_OE_DDR        DDRC
#define STP16_OE_PORT       PORTC
#define STP16_OE_MASK       1 << 2

#define PORT_MOSI		    PORTB
#define DDR_MOSI		    DDRB
#define MASK_MOSI		    (1 << 3)

#define PORT_SCK		    PORTB
#define DDR_SCK		        DDRB
#define MASK_SCK		    (1 << 5)

#define PORT_MISO		    PORTB
#define DDR_MISO		    DDRB
#define MASK_MISO		    (1 << 4)

#define PORT_SS		        PORTB
#define DDR_SS		        DDRB
#define MASK_SS		        (1 << 2)