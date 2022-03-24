#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#include "timer.h"
#include "errors.h"

#define TIM0_MAX 255

volatile static Time_t ticks = 0;

ISR(TIMER0_OVF_vect)
{
    ticks++;
    TCNT0 = (TIM0_MAX - 125);
}

Time_t TM_GetTicks()
{
    cli();
    Time_t _t = ticks;
    sei();
    
    return _t;
}

void TM_Delay(Time_t t)
{
    Time_t start = TM_GetTicks();

    while((TM_GetTicks() - start) < t) {
        _NOP();
    }
}


APP_Error_t TM_Init()
{

    TCNT0 = (TIM0_MAX - 125);
    TIMSK = (1 << TOIE0); // Enable ISR

    TCCR0 = (1 << CS01) | (1 << CS00); // 64x prescaller

    return APP_Ok;
}
