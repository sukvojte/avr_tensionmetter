#ifndef __PIN_CONFIG_H
#define __PIN_CONFIG_H



#define EN_DDR DDRC
#define EN_PORT PORTC
#define EN_PIN PC4

#define RS_DDR DDRC
#define RS_PORT PORTC
#define RS_PIN PC5

#define DATA_PORT PORTC
#define DATA_DDR DDRC
#define D0_PIN PC0
#define D1_PIN PC1
#define D2_PIN PC2
#define D3_PIN PC3


#define INT_DDR DDRD
#define INT_PORT PORTD
#define INT_PIN PD2
#define INT_INPUT PIND

#define CLK_DDR DDRB
#define CLK_PORT PORTB
#define CLK_PIN PB5

#define DT_DDR DDRB
#define DT_PORT PORTB
#define DT_PIN PB4

#define TX_DDR DDRD
#define TX_PORT PORTD
#define TX_PIN PD1

#define RX_DDR DDRD
#define RX_PORT PORTD
#define RX_PIN PD0

#endif // __PIN_CONFIG_H