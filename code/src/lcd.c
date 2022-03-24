#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"
#include "pin_config.h"

static const char PROGMEM LCD_own_chars[] = {
    0x02, 0x04, 0x00, 0x0C, 0x04, 0x04, 0x0E, 0x00,
    0x02, 0x04, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00,
    0x00, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00, 0x00,    
    0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00,
    0x1F, 0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x1F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    
};

static void write_n(uint8_t data)
{
    // DATA_PORT &= 0xF0;
    DATA_PORT = (DATA_PORT & 0xF0) | (data & 0x0F);

    // DATA_DDR |= 0x0F;
    // digitalWrite(RW_PIN, LOW); // write

    EN_PORT |= (1 << EN_PIN);
    _delay_us(3); // hold enable high for a us
    EN_PORT &= ~(1 << EN_PIN);
    _delay_us(3);
}

static void write(uint8_t data)
{
    write_n(data >> 4);
    write_n(data);
    _delay_us(50);
}

static void send_ins(uint8_t data)
{
    RS_PORT &= ~(1 << RS_PIN); // ins
    write(data);
}

static void send(uint8_t data)
{
    RS_PORT |= (1 << RS_PIN); // data
    write(data);
}

void LCD_set_position(uint8_t position)
{
    // LCD_position = position;
    if ((position & 0x10) > 0)
    {
        position &= 0x0F;
        position |= 0x40;
    }
    send_ins(0x80 | position);
}

void LCD_clear(void)
{
    send_ins(0x01);
    _delay_ms(2);
    LCD_set_position(0);
}

static void send_char(uint8_t data)
{
    static uint8_t last = 0;

    if ((data & 0x80) == 0)
    {
        // ascii
        send(data);
        return;
    }

    if ((data & 0xE0) == 0xC0)
    {
        // first octet
        last = data;
        return;
    }

    if ((data & 0xC0) == 0x80 && (last & 0xE0) == 0xC0)
    {
        // UTF-8 char
        uint16_t ch = (last << 8) | data;
        switch (ch)
        {
        case (uint16_t)'í':
            send(0x08);
            break;
        case (uint16_t)'á':
            send(0x09);
            break;
        default:
            send('?');
            break;
        }
        return;
    }
    return;
}

void LCD_string_P(const char *string)
{

    char c = pgm_read_byte_near(string);
    while (c)
    {
        send_char(c);
        string++;
        c = pgm_read_byte_near(string);
    }
}

void LCD_string(const char *string)
{
    while (*string)
    {
        send_char(*string);
        string++;
    }
}

APP_Error_t LCD_Init()
{
    DATA_PORT &= ~((1 << D0_PIN) | (1 << D1_PIN) | (1 << D2_PIN) | (1 << D3_PIN));
    EN_PORT &= ~(1 << EN_PIN);
    EN_DDR |= (1 << EN_PIN);
    RS_DDR |= (1 << RS_PIN);
    DATA_DDR |= (1 << D0_PIN) | (1 << D1_PIN) | (1 << D2_PIN) | (1 << D3_PIN);

    _delay_ms(30);

    send_ins(0x30); // goto 8bit
    _delay_ms(10);
    write_n(0x02); // goto 4bit
    _delay_ms(10);

    send_ins(0x20); // goto 4bit
    _delay_ms(10);

    send_ins(0x28); // set display size
    send_ins(0x08); // disable display
    LCD_clear();

    send_ins(0x0C); // enable display 0x3

    // send_ins(0x06); //  is entry mode it tells the lcd that we are going to use.

    send_ins(0x40); // write own characters

    uint16_t a;
    for (a = 0; a < sizeof(LCD_own_chars); a++)
    {
        send(pgm_read_byte_near(LCD_own_chars + a));
    }

    send_ins(0x80); // goto 1x1

    return APP_Ok;
}
