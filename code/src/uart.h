#ifndef __UART_H
#define __UART_H

#include "errors.h"

APP_Error_t UART_Init();
APP_Error_t UART_Send(const char *string);
APP_Error_t UART_Send_P(const char *string);

#endif // __UART_H