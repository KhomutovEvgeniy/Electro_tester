#pragma once

#include <stdint.h>

void adc_init( void );
void dma_adc_init ( void );
void adc_startConvertion( void );
uint16_t adc_getValue( uint8_t channel );
