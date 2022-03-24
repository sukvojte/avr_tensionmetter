
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"
#include "pin_config.h"
#include "config.h"

#define EnableDataEmptyISR() UCSRB |= (1 << UDRIE)
#define DisableDataEmptyISR() UCSRB &= ~(1 << UDRIE)

#define BUFF_SIZE 16
typedef struct
{
    volatile uint8_t data[BUFF_SIZE];
    volatile uint8_t wPos;
    volatile uint8_t sPos;
} Buff_t;

static Buff_t txBuffer;

ISR(USART_UDRE_vect)
{
    if (txBuffer.wPos != txBuffer.sPos)
    {
        UDR = txBuffer.data[txBuffer.sPos];
    }

    txBuffer.sPos++;
    if (txBuffer.sPos >= BUFF_SIZE)
    {
        txBuffer.sPos = 0;
    }

    if (txBuffer.wPos == txBuffer.sPos)
    {
        DisableDataEmptyISR();
    }
}

static APP_Error_t writeByte(uint8_t ch)
{
    cli();

    uint8_t nextPos = txBuffer.wPos + 1;
    if (nextPos >= BUFF_SIZE)
    {
        nextPos = 0;
    }

    if (nextPos == txBuffer.sPos)
    {
        sei();
        return APP_Error_Busy;
    }

    txBuffer.data[txBuffer.wPos] = ch;
    txBuffer.wPos = nextPos;

    sei();

    return APP_Ok;
}

APP_Error_t UART_Send_P(const char *string)
{   
    char *ch = (char *)string;
    char c = pgm_read_byte_near(ch);
    while (c)
    {
        switch (writeByte(c))
        {
        case APP_Error_Busy:
        case APP_Ok:
            break; // TODO: implement this
        }

        ch++;
        c = pgm_read_byte_near(ch);
    }
    
    EnableDataEmptyISR();

    return APP_Ok;
}

APP_Error_t UART_Send(const char *string)
{
    char *ch = (char *)string;
    uint16_t writed = 0;

    while (*ch)
    {
        switch (writeByte(*ch))
        {
        case APP_Error_Busy:
        case APP_Ok:
            break; // TODO: implement this
        }

        ch++;
        writed++;
    }

    EnableDataEmptyISR();

    return APP_Ok;
}

#define MYUBRR (F_CPU / 16 / BAUD - 1)

APP_Error_t UART_Init()
{

    TX_DDR |= (1 << TX_PIN);

    RX_DDR |= (1 << RX_PIN);
    // RX_DDR &= ~(1 << RX_PIN);

    UBRRH = (uint8_t)(MYUBRR >> 8);
    UBRRL = (uint8_t)MYUBRR;

    UCSRA = 0;
    UCSRB = (1 << TXEN);
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

    return APP_Ok;
}
