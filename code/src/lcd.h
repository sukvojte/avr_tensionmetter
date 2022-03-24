#ifndef __LCD_H
#define __LCD_H

#include "errors.h"

APP_Error_t LCD_Init();
void LCD_string(const char *string);
void LCD_string_P(const char *string);
void LCD_clear(void);
void LCD_set_position(uint8_t position);

#endif // __LCD_H