#pragma once

#include <avr/io.h>

#define F_CPU           16000000

// SPI Pin definitions
#define PORT_MOSI_0		PORTB
#define DDR_MOSI_0		DDRB
#define MASK_MOSI_0		(1 << 3)

#define PORT_SCK_0		PORTB
#define DDR_SCK_0		DDRB
#define MASK_SCK_0		(1 << 5)

#define PORT_MISO_0		PORTB
#define DDR_MISO_0		DDRB
#define MASK_MISO_0		(1 << 4)

#define PORT_SS_0		PORTB
#define DDR_SS_0		DDRB
#define MASK_SS_0		(1 << 2)

#define PORT_MOSI_1		PORTE
#define DDR_MOSI_1		DDRE
#define MASK_MOSI_1		(1 << 3)

#define PORT_SCK_1		PORTC
#define DDR_SCK_1		DDRC
#define MASK_SCK_1		(1 << 1)

#define PORT_MISO_1		PORTC
#define DDR_MISO_1		DDRC
#define MASK_MISO_1		(1 << 0)

#define PORT_SS_1		PORTE
#define DDR_SS_1		DDRE
#define MASK_SS_1		(1 << 2)
// End SPI Definitions

// SPI Devices SS Definitions
#define PORT_SS_FRONT_3	PORTC
#define DDR_SS_FRONT_3	DDRC
#define MASK_SS_FRONT_3	(1 << 3)

#define PORT_SS_FRONT_2	PORTC
#define DDR_SS_FRONT_2	DDRC
#define MASK_SS_FRONT_2	(1 << 2)

#define PORT_SS_FRONT_1	PORTE
#define DDR_SS_FRONT_1	DDRE
#define MASK_SS_FRONT_1	(1 << 2)
// End SPI Devices