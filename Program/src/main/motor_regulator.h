#pragma once

#include <stdint.h>

int32_t pid_regulator( int32_t err ); 
int32_t pi_regulator_axis_d( int32_t err );
int32_t pi_regulator_axis_q( int32_t err );
