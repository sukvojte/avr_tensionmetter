#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "pin_config.h"
#include "scale_adc.h"

#define EnableExt0Irq() GICR |= (1 << INT0)
#define DisableExt0Irq() GICR &= ~(1 << INT0)

static inline void addOneClock()
{
    CLK_PORT |= (1 << CLK_PIN);
    for (uint8_t i = 0; i < 12; i++)
    {
        _NOP();
    }

    CLK_PORT &= ~(1 << CLK_PIN);
    for (uint8_t i = 0; i < 12; i++)
    {
        _NOP();
    }
}

static uint8_t sampleFromStartCount = 0;
static volatile uint8_t convDone = 0;
static volatile uint8_t data[] = {0, 0, 0, 0};
static volatile uint8_t dataPos = 3;
static int32_t rawValue;
static float cumulativeValue;
static float offset;

ISR(SPI_STC_vect)
{
    dataPos--;
    data[dataPos + 1] = SPDR; // Read data
    if (dataPos == 0)
    {
        SPCR = 0; // Stop SPI
        convDone = 1;
        addOneClock();

        return;
    }
    SPDR = 0x00; // Start recv. 8bits
}

ISR(INT0_vect)
{
    RX_PORT ^= (1 << RX_PIN);
    if ((INT_INPUT & (1 << INT_PIN)) == 0 && (SPCR & (1 << SPE)) == 0)
    {
        dataPos = 3;
        DisableExt0Irq();
        SPCR = (1 << SPE) | (1 << SPIE) | (1 << MSTR) | (1 << CPHA) | (1 << SPR1); // Enable SPI transfer
        SPDR = 0x00;                                                 // Start recv. 8bits
    }
}

APP_Error_t SADC_Start()
{
    EnableExt0Irq();
    CLK_PORT &= ~(1 << CLK_PIN); // Enable ADC

    return APP_Ok;
}

APP_Error_t SADC_Stop()
{
    DisableExt0Irq();
    CLK_PORT |= (1 << CLK_PIN); // Send ADC to sleep
    SPCR = 0;                   // Stop SPI

    return APP_Ok;
}

APP_Error_t SADC_Init()
{
    INT_DDR &= ~(1 << INT_PIN); // as input
    INT_PORT |= (1 << INT_PIN); // as input

    DDRB |= (1 << PB2);
    DT_DDR &= ~(1 << DT_PIN); // input
    DT_PORT |= (1 << DT_PIN); // pullup

    CLK_DDR |= (1 << CLK_PIN);
    CLK_PORT |= (1 << CLK_PIN); // Send ADC to sleep

    cumulativeValue = 0;

    MCUCR &= ~((1 << ISC01) | (1 << ISC00)); // reset EXTI0
    MCUCR |= (1 << ISC01);                   // enable falling edge EXTI0

    return APP_Ok;
}

APP_Error_t SADC_Loop()
{
    uint8_t done = 0;
    /*
    if((INT_INPUT & (1 << INT_PIN)) == 0) {
        PORTD &= ~(1 << PD0);
    } else {
        PORTD |= (1 << PD0);
    }
*/
    cli();
    if (convDone == 1)
    {

        convDone = 0;
        rawValue = *((int32_t *)data);
        EnableExt0Irq();
        done = 1;
    }
    sei();

    if (done)
    {
        int64_t val = rawValue;
        val += 0x80000000;

        float f = (float)val;
        
        if (sampleFromStartCount == 0)
        {
            cumulativeValue = f; // startup value
        }

        cumulativeValue += (f - cumulativeValue) / 10.0; // Moving average
        //cumulativeValue = f; // TMP override

        if (sampleFromStartCount == 20)
        {
            offset = cumulativeValue; // set calibration offset
        }

        sampleFromStartCount++;
        if (sampleFromStartCount > 250)
        {
            sampleFromStartCount = 250;
        }
    }

    return APP_Ok;
}

uint8_t SADC_HasValidData()
{
    return sampleFromStartCount > 20;
}

float SADC_LastValue()
{
    //return cumulativeValue;
    // return cumulativeValue - offset;
    return (cumulativeValue - offset) / 12764000.0;
}// 6382  = 5 kg

uint32_t SADC_RawValue()
{
    return rawValue;
}
