#include "max7221.h"
#include <util/delay.h>
#define DATA_ASSERT()   (MAX7221_DATA_PORT  |= MAX7221_DATA_MASK)
#define DATA_DISABLE()  (MAX7221_DATA_PORT  &= ~MAX7221_DATA_MASK)
#define CS_ASSERT()     (MAX7221_CS_PORT    &= ~MAX7221_CS_MASK)
#define CS_DISABLE()    (MAX7221_CS_PORT    |= MAX7221_CS_MASK)
#define SCK_ASSERT()    (MAX7221_SCK_PORT   |= MAX7221_SCK_MASK)
#define SCK_DISABLE()   (MAX7221_SCK_PORT   &= ~MAX7221_SCK_MASK)

static void spi_write(const uint8_t* const arr, uint8_t len);

void max7221_init(void)
{
    MAX7221_CS_DDR |= MAX7221_CS_MASK;
    MAX7221_SCK_DDR |= MAX7221_SCK_MASK;
    MAX7221_DATA_DDR |= MAX7221_DATA_MASK;

    CS_ASSERT();
    SCK_ASSERT();
    DATA_ASSERT();

    for (uint8_t i = 0; i < MAX7221_DEVICES; ++i)
    {
        max7221_write(i, MAX7221_SCANLIMIT, 7);        // display 8 digits
        max7221_write(i, MAX7221_DECODEMODE, 0);       // we use software decoding for characters
        max7221_write(i, MAX7221_SHUTDOWN, 1);         // enable the chip
        max7221_write(i, MAX7221_INTENSITY, 8);        // set intensity to 8 (0 - 15)

        // clear the displays
        for (uint8_t j = MAX7221_DIGIT0; j <= MAX7221_DIGIT7; ++j)
            max7221_write(i, j, 0);
    }    
}

void max7221_write(uint8_t device, uint8_t address, uint8_t data)
{
    // buffer for storing commands, the devices which aren't addressed get a NOP command
    uint8_t arr[MAX7221_DEVICES*2] = { 0 };

    int8_t n = MAX7221_DEVICES - device - 1;
    if (n < 0 || n >= MAX7221_DEVICES)
        return;

    // store the command in the buffer
    arr[n*2] = address;
    arr[n*2 + 1] = data;

    spi_write(arr, MAX7221_DEVICES*2);
}

void max7221_display(uint8_t device, uint8_t digit, uint8_t c)
{
    if (c >= '0' && c <= '9')
        c -= '0';
    else if (c >= 'a' && c <= 'z')
        c -= 'a' - 10;
    else if (c >= 'A' && c <= 'Z')
        c -= 'A' - 10;
    else if (c > 0x0F)
        c = 0;

    static const uint8_t table[] =
    {
        0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B,         // 0 - 9
        0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47                                  // a - f
    };

    max7221_write(device, digit, table[c]);
}

static void spi_write(const uint8_t* const arr, uint8_t len)
{
    CS_ASSERT();

    for (uint8_t i = 0; i < len; ++i)
    {
        for (int8_t j = 7; j >= 0; --j)        // MAX7221 SPI is MSB (Big endian)
        {
            if ((arr[i] & (1 << j)) != 0)
                DATA_ASSERT();
            else
                DATA_DISABLE();

            SCK_ASSERT();
            SCK_DISABLE();
        }
    }

    CS_DISABLE();
}
