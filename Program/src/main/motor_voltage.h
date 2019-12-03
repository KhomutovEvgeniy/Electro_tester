#pragma once

#include <stdint.h>

void initPWM( void ); 

void tim_setDutyCycle(uint16_t channel, uint16_t dutyCycle);

