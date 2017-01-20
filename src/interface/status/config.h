#pragma once

#include <avr/io.h>

#define F_CPU           8000000

#define MAX7221_DEVICES 1
#define MAX7221_DECODE (1 << 0 | 0 << 1 | 1 << 2 | 1 << 3 | 1 << 4 | 0 << 5)

#define MAX7221_DATA_DDR    DDRA
#define MAX7221_DATA_PORT   PORTA
#define MAX7221_DATA_MASK   (1 << 1)

#define MAX7221_CS_DDR      DDRC
#define MAX7221_CS_PORT     PORTC
#define MAX7221_CS_MASK     (1 << 0)

#define MAX7221_SCK_DDR     DDRA
#define MAX7221_SCK_PORT    PORTA
#define MAX7221_SCK_MASK    (1 << 0)

#define POT_ADC (1 << 1)            // ADC 2
#define CLOCK_ADC (1 << 1 | 1 << 0) // ADC 3

#define PORT_MOSI		    PORTB
#define DDR_MOSI		    DDRB
#define MASK_MOSI		    (1 << 3)

#define PORT_SCK		    PORTB
#define DDR_SCK		        DDRB
#define MASK_SCK		    (1 << 5)

#define PORT_MISO		    PORTB
#define DDR_MISO		    DDRB
#define MASK_MISO		    (1 << 4)

#define PIN_SS		        PINB
#define PORT_SS		        PORTB
#define DDR_SS		        DDRB
#define MASK_SS		        (1 << 2)

#define ROT1_DDR    DDRB
#define ROT1_PIN    PINB
#define ROT1_PORT   PORTB
#define ROT1_MASK   (1 << 6)

#define ROT2_DDR    DDRD
#define ROT2_PIN    PIND
#define ROT2_PORT   PORTD
#define ROT2_MASK   (1 << 5)

#define ROT4_DDR    DDRB
#define ROT4_PIN    PINB
#define ROT4_PORT   PORTB
#define ROT4_MASK   (1 << 7)

#define ROT8_DDR    DDRD
#define ROT8_PIN    PIND
#define ROT8_PORT   PORTD
#define ROT8_MASK   (1 << 6)

