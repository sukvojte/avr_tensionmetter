#include "lcd.h"
#include "scale_adc.h"
#include <avr/cpufunc.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
#include "config.h"
#include "pin_config.h"
#include "timer.h"
#include "uart.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int main()
{
    static Time_t lastLcdUpdate = 0;
    char buff[20];

    TM_Init();

    UART_Init();
    UART_Send_P(PSTR("init\n"));

    LCD_Init();
    LCD_clear();
    SADC_Init();

    SADC_Start();
    UART_Send_P(PSTR("started\n"));

    sei(); // Enable IRQ

    for(;;) {

        SADC_Loop();

        if ((TM_GetTicks() - lastLcdUpdate) > LCD_UPDATE_INTERVAL)
        {
            UART_Send_P(PSTR("loop\n"));
            lastLcdUpdate = TM_GetTicks();

            if(SADC_HasValidData()) {
            //if(0) {
                ADCSRA = 0;
                LCD_set_position(0);
                LCD_string_P(PSTR("Síla:         Kg"));

                LCD_set_position(7);
                float val = SADC_LastValue();
                dtostrf(val, 6, 2, buff);
                LCD_string(buff);

                LCD_set_position(16);
                uint8_t v = (max(val, 0.0)/50.0) * 16;

                for(uint8_t i = 0; i < 16; i++){                    
                    buff[i] = i <= v ? 0x0C : 0x0B;
                }
                buff[16] = 0;
                LCD_string(buff);
                
            } else {
                LCD_set_position(0);
                LCD_string_P(PSTR("Kalibruji váhu"));

                ADMUX = (1 << REFS0) | (1 << MUX3)  | (1 << MUX2)  | (1 << MUX1);
                ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

                while(!(ADCSRA & (1 << ADIF)));

                float voltage = 1.3 / (((float)ADC) / 1023.0);

                dtostrf(voltage, 4, 2, buff);

                LCD_set_position(16);
                LCD_string_P(PSTR("  baterie:     V"));
                LCD_set_position(26);
                LCD_string(buff);
                TM_Delay(500);
            }
        }
            
            
    }
    return 0;
}
