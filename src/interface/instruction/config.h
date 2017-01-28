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

#define MOSI_DDR	DDRB
#define MOSI_PORT	PORTB
#define MOSI_MASK	(1 << 3)

#define SCK_DDR		DDRB
#define SCK_PORT	PORTB
#define SCK_MASK	(1 << 5)

#define MISO_DDR	DDRB
#define MISO_PORT	PORTB
#define MISO_MASK	(1 << 4)

#define SS_DDR		DDRB
#define SS_PIN		PINB
#define SS_PORT		PORTB
#define SS_MASK     (1 << 2)