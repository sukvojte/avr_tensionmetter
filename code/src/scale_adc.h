#ifndef __SCALE_ADC_H
#define __SCALE_ADC_H

#include <stdint.h>
#include "errors.h"

APP_Error_t SADC_Init();
APP_Error_t SADC_Start();
APP_Error_t SADC_Stop();
APP_Error_t SADC_Loop();
float SADC_LastValue();
uint8_t SADC_HasValidData();
uint32_t SADC_RawValue();

#endif
