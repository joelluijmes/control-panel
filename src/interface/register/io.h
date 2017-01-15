#pragma once

#include <avr/io.h>

#define BUTTON_DDR          DDRD
#define BUTTON_PIN          PIND
#define BUTTON_MASK         (1 << 0)

#define MAX7221_DEVICES 1

#define MAX7221_DATA_DDR    DDRB
#define MAX7221_DATA_PORT   PORTB
#define MAX7221_DATA_MASK   (1 << 1)

#define MAX7221_CS_DDR      DDRB
#define MAX7221_CS_PORT     PORTB
#define MAX7221_CS_MASK     (1 << 0)

#define MAX7221_SCK_DDR     DDRD
#define MAX7221_SCK_PORT    PORTD
#define MAX7221_SCK_MASK    (1 << 7)

#define MUX4067_S1_DDR      DDRD
#define MUX4067_S1_PORT     PORTD
#define MUX4067_S1_MASK     (1 << 6)

#define MUX4067_S2_DDR      DDRD
#define MUX4067_S2_PORT     PORTD
#define MUX4067_S2_MASK     (1 << 5)

#define MUX4067_S3_DDR      DDRD
#define MUX4067_S3_PORT     PORTD
#define MUX4067_S3_MASK     (1 << 2)

#define MUX4067_S4_DDR      DDRD
#define MUX4067_S4_PORT     PORTD
#define MUX4067_S4_MASK     (1 << 3)

#define MUX4067_EN_DDR      DDRD
#define MUX4067_EN_PORT     PORTD
#define MUX4067_EN_MASK     (1 << 1)

#define MUX4067_ADC_MASK    2

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