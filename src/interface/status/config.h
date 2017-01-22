#pragma once

#include <avr/io.h>

#define F_CPU           8000000

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

#define LED_CLK_DDR     DDRA
#define LED_CLK_PORT    PORTA
#define LED_CLK_MASK    (1 << 3)
