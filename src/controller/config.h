#pragma once

#include <avr/io.h>

#define F_CPU           16000000

// SPI Pin definitions
#define MOSI0_PORT		PORTB
#define MOSI0_DDR		DDRB
#define MOSI0_MASK		(1 << 3)

#define SCK0_PORT		PORTB
#define SCK0_DDR		DDRB
#define SCK0_MASK		(1 << 5)

#define MISO0_PORT		PORTB
#define MISO0_DDR		DDRB
#define MISO0_MASK		(1 << 4)

#define SS0_PORT	    PORTB
#define SS0_DDR		    DDRB
#define SS0_MASK	    (1 << 2)

#define MOSI1_PORT		PORTE
#define MOSI1_DDR		DDRE
#define MOSI1_MASK		(1 << 3)

#define SCK1_PORT		PORTC
#define SCK1_DDR		DDRC
#define SCK1_MASK		(1 << 1)

#define MISO1_PORT		PORTC
#define MISO1_DDR		DDRC
#define MISO1_MASK		(1 << 0)

#define SS1_PORT	    PORTE
#define SS1_DDR		    DDRE
#define SS1_MASK	    (1 << 2)
// End SPI Definitions

// SPI Devices SS Definitions
#define FRONT_SS1_DDR	DDRE
#define FRONT_SS1_PORT	PORTE
#define FRONT_SS1_MASK	(1 << 2)

#define FRONT_SS3_DDR	DDRC
#define FRONT_SS3_PORT	PORTC
#define FRONT_SS3_MASK	(1 << 3)

#define FRONT_SS2_DDR	DDRC
#define FRONT_SS2_PORT	PORTC
#define FRONT_SS2_MASK	(1 << 2)
// End SPI Devices

#define MCP23_SS12_PORT	PORTB
#define MCP23_SS12_DDR	DDRB
#define MCP23_SS12_MASK	(1 << 0 | 1 << 2)

#define MCP23_SS3_PORT	PORTC
#define MCP23_SS3_DDR	DDRC
#define MCP23_SS3_MASK	(1 << 4 | 1 << 2)

#define CLK_DDR	        DDRB
#define CLK_PORT	    PORTB
#define CLK_MASK	    (1 << 1)

#define BREAK_DDR	    DDRD
#define BREAK_PORT	    PORTD
#define BREAK_MASK	    (1 << 2)

