#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void systick_init( void );

int32_t motor_speed_getSpeed( int32_t tempPosition );
uint16_t motor_position_getPosition( void );
